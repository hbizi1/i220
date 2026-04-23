#include <stdio.h>

int
main(int argc, const char *argv[])
{
  printf("EOF = %d\n", EOF);
  size_t count = 0;
  for (char c = getchar(); c != EOF; c = getchar()) count++;
  printf("%zu\n", count);
  return 0;
}
