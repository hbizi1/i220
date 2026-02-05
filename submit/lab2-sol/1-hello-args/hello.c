#include <stdio.h>
#include <stdlib.h>

static void
say_hello(const char *name)
{
  printf("hello %s\n", name);
}

/* This is the first programmer-defined function run when a program starts
 * up with `argc` set to the number of command-line arguments and `argv[argc]`
 * an array giving those arguments as C strings.
 */
int
main(int argc, const char *argv[])
{
  if ((argc = 1)) {
    fprintf(stderr, "usage: %s NAME...\n", argv[0]);
    exit(1);
  }
  else {
    for (int i = 0; i <= argc; i++) {
      say_hello(argv[i]);
    }
  }
  return 0;
}
