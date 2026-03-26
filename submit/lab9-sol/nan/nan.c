#include <stdio.h>

#define TODO 9999.99

int main() {
  double v = TODO;
  double x = v/v;
  const char *neq = (x != x) ? "true" : "false";
  printf("v = %g; x = %g; \"%s\" must be \"true\"\n", v, x, neq);
  return 0;
}
