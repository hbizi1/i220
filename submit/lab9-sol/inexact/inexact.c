#include <math.h>
#include <stdio.h>

#ifdef USE_FLOAT
typedef float Real;
#else
typedef double Real;
#endif

#define TODO 0

// replace TODO below with 0.1
int main() {
  const Real x = TODO;             //value being summed repeatedly
  const int n = ceil(1.0/x);      //how many additions to sum to 1.0
  Real sum = 0;
  for (int j = 0; j < n; j++) {
    sum += x;
  }
  //sum should be x + x + ... + x (n-times); should be equal to 1
  const char * sum_is_one = (sum == 1.0) ? "true" : "false";
  printf("%8g * %2d == %0.20f; sum-is-1: %s\n", x, n, sum, sum_is_one);
}
