#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef N_ERR
#define N_ERR 4
#endif

//fgetc() wrapper which forces an error by reopening stream with mode "a".
__attribute__((unused))
static int fgetc_err(FILE *stream) {
  static int nc = 0;
  if (nc++ == N_ERR) freopen(NULL, "a", stream);
  return fgetc(stream);
}

/** copy stream in (corresponding to inName) to stream out (corresponding
 *  to outName).  Return non-zero on error.
 */
static int
do_copy(const char *inName, FILE *in, const char *outName, FILE *out)
{
  int c;
  while ((c = fgetc(in)) != EOF) {
    if (fputc(c, out) == EOF) {
      fprintf(stderr, "error writing to %s: %s\n", outName, strerror(errno));
      return 1; //return rather than exit to ensure files closed
    }
  }
  return 0;
}

/** Copy file specified by first argument to file specified by
 *  second argument.
 */
int
main(int argc, const char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "usage: %s SRC_NAME DEST_NAME\n", argv[0]);
    exit(1);
  }
  const char *srcName = argv[1];
  const char *destName = argv[2];
  FILE *in = fopen(srcName, "r");
  if (!in) {
    fprintf(stderr, "cannot read %s: %s\n", srcName, strerror(errno));
    exit(1);
  }
  FILE *out = fopen(destName, "w");
  if (!out) {
    fprintf(stderr, "cannot write %s: %s\n", destName, strerror(errno));
    exit(1);
  }
  int status = do_copy(srcName, in, destName, out);
  if (fclose(in) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", srcName, strerror(errno));
    exit(1);
  }
  if (fclose(out) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", destName, strerror(errno));
    exit(1);
  }
  return status;
}
