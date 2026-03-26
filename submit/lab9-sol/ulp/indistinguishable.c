#include <stdio.h>

#define TODO 0

int main()
{
  const double diff = 500.0;

  const unsigned d_shift = TODO;
  double d = 1ull << d_shift;
  double d1 = d + diff;
  const char *d_indistinguishable = (d == d1) ? "true" : "false";
  printf("for d = 2**%d, indisguishable(d, d + %d) == %s\n",
         d_shift, (int)diff, d_indistinguishable);

  const unsigned f_shift = TODO;
  float f = 1ull << f_shift;
  float f1 = f + diff;
  const char *f_indistinguishable = (f == f1) ? "true" : "false";
  printf("for f = 2**%d, indisguishable(f, f + %d) == %s\n",
         f_shift, (int)diff, f_indistinguishable);
}
