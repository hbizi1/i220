#include <stdio.h>

int main() {
  int stkVar;
  void *rsp = 0;

  __asm__("movq (%%rsp), %0": "=stkVAR"(rsp));

  printf("&stkVar = %p; rsp = %p\n", &stkVar, rsp);
  printf("diff:  &stkVar - %%rsp = %zu\n", (char *)&stkVar -  (char *)rsp);
  return 0;
}
