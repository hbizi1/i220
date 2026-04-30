#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>


void disasm_fn();
void end_fn();


int main() {
  const char *start = (char*)disasm_fn;
  const char *end = (char*)end_fn;
  ZydisDecoder decoder;
  ZyanStatus status = ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64,
                                       ZYDIS_STACK_WIDTH_64);
  if (!ZYAN_SUCCESS(status)) assert("cannot init decoder" && 0);
  size_t len = 0;
  for (const char *p = start; p < end; p += len) {
    ZydisDecodedInstruction instr;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
    status = ZydisDecoderDecodeFull(&decoder, p, 15, &instr, operands);
    if (!ZYAN_SUCCESS(status)) assert("cannot decode" && 0);
    len = instr.length;
    printf("opcode =%02x; len = %zu; operand-size = %u; ",
           instr.opcode, len, operands[0].size);
    for (int j = 0; j < instr.operand_count_visible; j++) {
      const ZydisDecodedOperand op = operands[j];
      const char *act = op.actions == ZYDIS_OPERAND_ACTION_READ
        ? "read"
        : op.actions == ZYDIS_OPERAND_ACTION_WRITE ? "write" : "unknown";
      printf("action = %s(%u) ", act, op.actions);
      switch (op.type) {
      case ZYDIS_OPERAND_TYPE_REGISTER:
        printf("reg %s; ", ZydisRegisterGetString(op.reg.value));
        break;
      case ZYDIS_OPERAND_TYPE_MEMORY:
        printf("mem; ");
        break;
      case ZYDIS_OPERAND_TYPE_IMMEDIATE:
        printf("imm %lx; ", op.imm.value.u);
        break;
      default:
        assert("unhandled operand type" && 0);
      }
    }
    printf("\n");

  }
}

#if 0
int main()
{
  /*
    ZyanU8 data[] =
    {
        0x51, 0x8D, 0x45, 0xFF, 0x50, 0xFF, 0x75, 0x0C, 0xFF, 0x75,
        0x08, 0xFF, 0x15, 0xA0, 0xA5, 0x48, 0x76, 0x85, 0xC0, 0x0F,
        0x88, 0xFC, 0xDA, 0x02, 0x00
    };
  */
    // The runtime address (instruction pointer) was chosen arbitrarily here in order to better
    // visualize relative addressing. In your actual program, set this to e.g. the memory address
    // that the code being disassembled was read from.
  ZyanU64 runtime_address = 0;

    // Loop over the instructions in our buffer.
    ZyanUSize len = 0;
    ZydisDisassembledInstruction instruction;
    const ZyanU64 *start = (ZyanU64*)disasm_fn;
    const ZyanU64 *end = (ZyanU64*)end_fn;
    for (const ZyanU64 *p= start; p < end; p = (ZyanU64*) ((char *)p + len)) {
      const ZyanStatus status =
        ZydisDisassembleATT(ZYDIS_MACHINE_MODE_LONG_64, runtime_address,
                            p, 15, &instruction);
      if (ZYAN_SUCCESS(status)) {
        len = instruction.info.length;
        printf("%016" PRIX64 "  %s; len = %lu\n",
               runtime_address, instruction.text, len);
        runtime_address += len;
      }
    }

    return 0;
}
#endif
