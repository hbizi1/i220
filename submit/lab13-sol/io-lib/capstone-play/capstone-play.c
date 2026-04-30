#include <capstone/capstone.h>

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void disasm_fn();
void end_fn();

int main(int argc, const char *argv[])
{
  csh handle;
  if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
    fprintf(stderr, "cannot open capstone: %s\n", strerror(errno));
    exit(1);
  }
  cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
  cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
  const uint8_t *start = (uint8_t *)disasm_fn;
  const uint8_t *end = (uint8_t *) end_fn;
  cs_insn *insn;
  int count = cs_disasm(handle, start, end-start, 0x1000, 0, &insn);
  if (count > 0) {
    for (int j = 0; j < count; j++) {
      printf("size: %u; 0x%"PRIx64":\t%s\t\t%s\n",
             insn[j].size, insn[j].address, insn[j].mnemonic, insn[j].op_str);
      // Print all registers accessed by this instruction.
      uint8_t read_count, write_count;
      cs_regs regs_read, regs_write;
      if (cs_regs_access(handle, &insn[j],  regs_read, &read_count,
            regs_write, &write_count) == 0) {
        if (read_count > 0) {
          printf("\n\tRegisters read:");
          for (int i = 0; i < read_count; i++) {
                printf(" %s", cs_reg_name(handle, regs_read[i]));
          }
          printf("\n");
        }
        if (write_count > 0) {
          printf("\n\tRegisters modified:");
          for (int i = 0; i < write_count; i++) {
            printf(" %s", cs_reg_name(handle, regs_write[i]));
          }
          printf("\n");
        }
      }
      cs_x86 x86 = insn[j].detail->x86;
      unsigned n_operands = x86.op_count;
      printf("operand count: %u\n", n_operands);
      for (int k = 0; k < n_operands; k++) {
        printf("size: %u: ", x86.operands[k].size);
        switch (x86.operands[k].type) {
        case X86_OP_REG:
          printf("reg %d; ", x86.operands[k].reg);
          break;
        case X86_OP_IMM:
          printf("imm %lx; ", x86.operands[k].imm);
          break;
        case X86_OP_MEM:
          printf("mem; ");
          break;
        default:
          assert(0);
        }
      }
      printf("\n");
    }
    cs_free(insn, count);
  }
  else {
    fprintf(stderr, "failed to disasemble\n");
    exit(1);
  }
  cs_close(&handle);
}
