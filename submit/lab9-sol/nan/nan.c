#include <stdio.h>

int main() {
  double v = 0.0;
  double x = v/v;
  const char *neq = (x != x) ? "true" : "false";
  printf("v = %g; x = %g; \"%s\" must be \"true\"\n", v, x, neq);
  return 0;
}
