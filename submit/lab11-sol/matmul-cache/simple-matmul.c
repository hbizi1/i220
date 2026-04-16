#include "matmul.h"

void
matrix_multiply(int n, long a[][n], long b[][n], long c[][n])
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      c[i][j] = 0;
      for (int k = 0; k < n; k++) {
	int x = b[k][j];
	c[i][j] += a[i][k]*x;
      }
    }
  }
}

