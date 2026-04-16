#include "matmul.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Fill in matrix t[n][n] as the transpose of matrix a[n][n].  That is,
 *  for all i, j set t[i][j] to a[j][i].
 */
static void
matrix_transpose(int n, long a[][n], long t[][n])
{
  //@TODO add your code here
}


void
matrix_multiply(int n, long a[][n], long b[][n], long c[][n])
{
  long (*tmp)[n] = malloc(sizeof(long[n][n]));
  if (!tmp) {
    fprintf(stderr, "could not malloc transpose matrix: %s\n",
            strerror(errno));
    exit(1);
  }
  matrix_transpose(n, b, tmp);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      c[i][j] = 0;
      for (int k = 0; k < n; k++) {
        c[i][j] += a[i][k]*tmp[j][k];
      }
    }
  }
  free(tmp);
}
