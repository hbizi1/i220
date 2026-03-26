#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_FLOAT
typedef float Real;
typedef int Integer;
#define Log2 log2f
#else
typedef double Real;
typedef long long Integer;
#define Log2 log2
#endif

//f and i occupy same memory locations so that assigning to i changes f
typedef union {
  Real f;
  Integer i;
} FloatInt;

enum { DATA_T, LG_DATA_T, VERBOSE_T };
const char *types[] = { //must be in same order as above enum
  "data", "lg-data", "verbose",
};


static void
usage(const char *progName)
{
  fprintf(stderr, "usage %s ", progName);
  const int n = sizeof(types)/sizeof(types[0]); //# of elements in types[]
  for (int i = 0; i < n; i++) {
    fprintf(stderr, "%s%s", types[i], (i < (n - 1)) ? "|" : "\n");
  }
  exit(1);
}

/** Return enum corresponding to type.  < 0 if not found. */
static int
get_type(const char *type)
{
  for (int i = 0; i < sizeof(types)/sizeof(types[0]); i++) {
    if (strcmp(types[i], type) == 0) { //strcmp() == 0 means equal
      return i;
    }
  }
  return -1;
}

static
void out_ulps(int type)
{
  for (int i = 0; i < sizeof(long long)*CHAR_BIT; i++) {
    const Real f = 0x1ull << i;
    FloatInt fi = {.f = f };
    fi.i += 1;  //change ulp of fi.f
    switch (type) {
      case DATA_T:
        printf("%g %g\n", f, (fi.f - f));
        break;
      case LG_DATA_T:
        printf("%g %g\n", Log2(f), Log2(fi.f - f));
        break;
      case VERBOSE_T:
        printf("value = 2**%d (%g), ulp = %.14g\n", i, f, fi.f - f);
        break;
    }
  }
}

int main(int argc, const char *argv[]) {
  assert(sizeof(Real) == sizeof(Integer)); //validate essential assumption
  int type;
  if (argc != 2 || (type = get_type(argv[1])) < 0) usage(argv[0]);
  out_ulps(type);
  return 0;
}
