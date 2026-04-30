//inspired by <https://medium.com/@tom_84912/diy-memory-mapped-i-o-memory-mapped-fifos-ad53bcb4df8e>

#define _GNU_SOURCE 1 //for REG_RIP

#include "active-mem.h"

#include <Zydis/Zydis.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <signal.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <unistd.h>

typedef struct {
  char *reg_name;
  uint8_t reg_n;
} RegInfo;

const RegInfo REG_INFOS[] = {
  { "al", REG_RAX },
  { "ah", REG_RAX },
  { "ax", REG_RAX },
  { "eax", REG_RAX },
  { "rax", REG_RAX },
  { "bl", REG_RBX },
  { "bh", REG_RBX },
  { "bx", REG_RBX },
  { "ebx", REG_RBX },
  { "rbx", REG_RBX },
  { "cl", REG_RCX },
  { "ch", REG_RCX },
  { "cx", REG_RCX },
  { "ecx", REG_RCX },
  { "rcx", REG_RCX },
  { "dl", REG_RDX },
  { "dh", REG_RDX },
  { "dx", REG_RDX },
  { "edx", REG_RDX },
  { "rdx", REG_RDX },
  { "sil", REG_RSI },
  { "si", REG_RSI },
  { "esi", REG_RSI },
  { "rsi", REG_RSI },
  { "dil", REG_RDI },
  { "di", REG_RDI },
  { "edi", REG_RDI },
  { "rdi", REG_RDI },
  { "bpl", REG_RBP },
  { "bp", REG_RBP },
  { "ebp", REG_RBP },
  { "rbp", REG_RBP },
  { "spl", REG_RSP },
  { "sp", REG_RSP },
  { "esp", REG_RSP },
  { "rsp", REG_RSP },
  { "r8b", REG_R8 },
  { "r8w", REG_R8 },
  { "r8d", REG_R8 },
  { "r8", REG_R8 },
  { "r9b", REG_R9 },
  { "r9w", REG_R9 },
  { "r9d", REG_R9 },
  { "r9", REG_R9 },
  { "r10b", REG_R10 },
  { "r10w", REG_R10 },
  { "r10d", REG_R10 },
  { "r10", REG_R10 },
  { "r11b", REG_R11 },
  { "r11w", REG_R11 },
  { "r11d", REG_R11 },
  { "r12", REG_R12 },
  { "r12b", REG_R12 },
  { "r12w", REG_R12 },
  { "r12d", REG_R12 },
  { "r13", REG_R13 },
  { "r13b", REG_R13 },
  { "r13w", REG_R13 },
  { "r13d", REG_R13 },
  { "r14", REG_R14 },
  { "r14b", REG_R14 },
  { "r14w", REG_R14 },
  { "r14d", REG_R14 },
  { "r14", REG_R14 },
  { "r15b", REG_R15 },
  { "r15w", REG_R15 },
  { "r15d", REG_R15 },
  { "r15", REG_R15 },
};

static unsigned
reg_name_to_reg_n(const char *reg_name)
{
  for (int i = 0; i < sizeof(REG_INFOS)/sizeof(REG_INFOS[0]); i++) {
    const RegInfo *p = &REG_INFOS[i];
    if (strcmp(p->reg_name, reg_name) == 0) {
      return p->reg_n;
    }
  }
  assert(0 && "no reg_n found");
  return 0;
}

typedef enum { MEM_TO_REG_OP, REG_TO_MEM_OP, IMM_TO_MEM_OP } OpType;

typedef struct {
  const void *addr;
  size_t op_size;      //# of bytes in instruction
  size_t operand_size; //# of bytes in each operand
  OpType op_type;
  int reg_n;           //source or dest reg_n; suitable to index mcontext.gregs[]
  MemVal immed;        //source immed
} OpInfo;

static OpInfo x86_op_info(const char *addr)
{
  OpInfo op_info = { .addr = addr, };
  ZydisDecoder decoder;
  ZyanStatus status = ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64,
                                       ZYDIS_STACK_WIDTH_64);
  if (!ZYAN_SUCCESS(status)) assert("cannot init decoder" && 0);
  ZydisDecodedInstruction instr;
  ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
  status = ZydisDecoderDecodeFull(&decoder, addr, 15, &instr, operands);
  if (!ZYAN_SUCCESS(status)) assert("cannot decode" && 0);
  op_info.op_size = instr.length;
  assert(instr.operand_count_visible == 2 && "2 operands expected");
  op_info.operand_size = operands[0].size;
  const unsigned op1_type = operands[1].type;
  const OpType op_type =
    (op1_type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
    ? IMM_TO_MEM_OP
    : (op1_type == ZYDIS_OPERAND_TYPE_REGISTER)
    ? REG_TO_MEM_OP
    : MEM_TO_REG_OP;
  op_info.op_type = op_type;
  if (op_type == IMM_TO_MEM_OP) {
    op_info.immed = operands[1].imm.value.u;
  }
  else {
    ZydisDecodedOperand reg_operand = operands[op_type == REG_TO_MEM_OP ? 1 : 0];
    const char *reg_name = ZydisRegisterGetString(reg_operand.reg.value);
    op_info.reg_n = reg_name_to_reg_n(reg_name);
  }
  return op_info;
}

typedef struct {
  void *ctx;           //client's ctx
  MemReadFn *read_fn;  //function to be called on intercepted read
  MemWriteFn *write_fn;//function to be called on intercepted write
  void *lo_addr;       //lo address (inclusive) of allocated memory
  void *hi_addr;       //hi address (exclusive) of allocated memory
} ActiveMemInfo;

// using global var because we are storing base_addr and size which we
// need in handler.  Alternately, do not store base_addr and size and
// store ActiveMemInfo in previous page with some magic #.
static ActiveMemInfo mem_info;


static void segv_handler(int sig_n, siginfo_t *info, void *uctx)
{
  const void *access_addr = info->si_addr;
  mcontext_t *mctx = &((ucontext_t *)uctx)->uc_mcontext;

  //will not work under os/x
  const char *op_addr = (const char *)mctx->gregs[REG_RIP];

  assert(mem_info.lo_addr <= access_addr && access_addr < mem_info.hi_addr &&
         "unexpected SEGV");

  const OpInfo op_info = x86_op_info(op_addr);

  //set up ip to point to next instruction
  mctx->gregs[REG_RIP] += op_info.op_size;

  switch (op_info.op_type) {
   case MEM_TO_REG_OP:
     mctx->gregs[op_info.reg_n] = mem_info.read_fn(mem_info.ctx, access_addr);
     break;
   case REG_TO_MEM_OP:
     mem_info.write_fn(mem_info.ctx, access_addr,
                       mctx->gregs[op_info.reg_n]);
     break;
   case IMM_TO_MEM_OP:
     mem_info.write_fn(mem_info.ctx, access_addr, op_info.immed);
     break;
   default:
     assert(0 && "unexpected opcode");
   }


}


/** allocate n_bytes of active memory with read attempts resulting in
 *  calls to read_fn and write attempts resulting in calls to write_fn.
 *  Returns NULL on failure.
 */
void *active_mem_calloc(size_t n_bytes, void *ctx,
                        MemReadFn *read_fn,  MemWriteFn *write_fn)
{
  long pagesize = sysconf(_SC_PAGESIZE);
  if (pagesize < 0) return NULL;

  struct sigaction sa = { .sa_flags = SA_SIGINFO, .sa_sigaction = segv_handler };
  if (sigaction(SIGSEGV, &sa, NULL) < 0) return NULL;

  void *mem = mmap(NULL, n_bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == (void *)-1) return NULL;
  mem_info = (ActiveMemInfo){
    .ctx = ctx, .read_fn = read_fn, .write_fn = write_fn,
    .lo_addr = mem, .hi_addr = mem + n_bytes,
  };
  return mem;

  /* if (!mem) return NULL; */
  /* if (posix_memalign(&mem, pagesize, n_bytes) != 0) return NULL; */
  /* if (memset(mem, 0, n_bytes) != mem) return NULL; */
  /* if (mprotect(mem, n_bytes, PROT_NONE) != 0) { free(mem); return NULL; } */
  /* return mem; */
}

/** free previously allocated n_bytes of active memory at p.  Returns
 *  non-zero on error.
 */
int
active_mem_free(void *p, size_t n_bytes)
{
  return munmap(p, n_bytes);
}



#ifdef TEST_ACTIVE_MEM

#include <errno.h>
#include <stdio.h>

typedef struct {
  volatile char c;
  volatile short s;
  volatile int i;
  volatile long g;
} Data;

typedef struct {  Data r, w, i; } ActiveData;


static MemVal read_fn(void *ctx, const void *addr) {
  printf("reading %p\n", addr);
  return 23;
}

static void write_fn(void *ctx, const void *addr, MemVal val) {
  printf("writing %lld to %p\n", val, addr);
}

int f(int n);

int main() {
  ActiveData *d = active_mem_calloc(sizeof(ActiveData), NULL, read_fn, write_fn);
  if (!d) {
    fprintf(stderr, "cannot create active_mem: %s\n", strerror(errno));
    exit(1);
  }

  d->i.g = 123456;
  d->w.g = f(2) + 5;
  long  g = d->r.g;

  d->i.i =  33;
  d->w.i = f(5);
  int i = d->r.i;

  d->i.s = 22;
  d->w.s = f(10);
  short s = d->r.s;

  d->i.c = 11;
  d->w.c = f(12);
  char c = d->r.c;

  printf("%ld %d %d %d\n", g, i, s, c);

  if (active_mem_free(d, sizeof(ActiveData)) != 0) {
    fprintf(stderr, "cannot free active_mem: %s\n", strerror(errno));
    exit(1);
  }
}

#endif
