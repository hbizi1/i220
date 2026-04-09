#include <stdio.h>

int main() {
  int stkVar;
  void *rsp = 0;

  //TODO: inline assembly to set rsp C var to value of %rsp register

  printf("&stkVar = %p; rsp = %p\n", &stkVar, rsp);
  printf("diff:  &stkVar - %%rsp = %zu\n", (char *)&stkVar -  (char *)rsp);
  return 0;
}
