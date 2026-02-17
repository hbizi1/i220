#include "file-utils.h"
#include "morse.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void
morseEncode(FILE *in, FILE *out)
{
  Byte *text;
  int nChars = readFile(in, &text);
  if (nChars < 0) {
    fprintf(stderr, "cannot read input file\n");
    exit(1);
  }
  //worst case is text is alternating '0' words; each 0 is 5 dashes
  //"-----".  The number of bits will be # of bits for first 4 dashes
  //(each dash will require 3 bits for dash + 1 bit for inter-symbol
  //marker) + # of bits for last dash (3) + # of bits for
  //inter-word marker (7).  Hence, we will have 4*4 + 3 + 7 == 26
  //bits.  We need space for 2 extra chars to accommodate the `AR`
  //prosign terminator.
  int maxNBytes = ((nChars + 2) * 26 + BITS_PER_BYTE - 1)/BITS_PER_BYTE;
  Byte *bytes = calloc(1, maxNBytes);
  if (bytes == NULL) {
    fprintf(stderr, "cannot alloc bytes: %s\n", strerror(errno));
    exit(1);
  }
  int nBytes = text_to_morse(text, nChars, bytes);
  free(text);
  if (nBytes < 0) {
    fprintf(stderr, "cannot encode text\n");
    exit(1);
  }
  if (writeFile(bytes, nBytes, out) != nBytes) {
    fprintf(stderr, "cannot write output\n");
    exit(1);
  }
  free(bytes);
}

static void
morseDecode(FILE *in, FILE *out)
{
  Byte *bytes;
  int nBytes = readFile(in, &bytes);
  if (nBytes < 0) {
    fprintf(stderr, "cannot read input file\n");
    exit(1);
  }
  //worst case is text is all 'E's; each E is 1 dot which will be
  //1 (for dot) + 3 bits (for inter-character marker) == 4
  int maxNChars =
    2*(nBytes * (BITS_PER_BYTE/4)) + 1; //(+ 1 for terminating '\0')
  Byte *text = malloc(maxNChars);
  if (text == NULL) {
    fprintf(stderr, "cannot alloc text: %s\n", strerror(errno));
    exit(1);
  }
  int nChars = morse_to_text(bytes, nBytes, text);
  free(bytes);
  if (nChars < 0) {
    fprintf(stderr, "cannot decode bytes\n");
    exit(1);
  }
  if (writeFile(text, nChars, out) != nChars) {
    fprintf(stderr, "cannot write output\n");
    exit(1);
  }
  free(text);
}

int
main(int argc, const char *argv[])
{
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "usage: %s SRC_FILE [DEST_FILE]\n", argv[0]);
    exit(1);
  }
  const char *lastSlash = strrchr(argv[0], '/');
  const char *basename = (lastSlash == NULL) ? argv[0] : lastSlash + 1;
  const int isEncode = strcmp(basename, "morse-encode") == 0;
  const char *inMode = isEncode ? "r" : "rb";
  FILE *in = fopen(argv[1], inMode);
  if (!in) {
    fprintf(stderr, "cannot read %s: %s\n", argv[1], strerror(errno));
    exit(1);
  }
  const char *outMode = isEncode ? "wb" : "w";
  FILE *out = (argc == 3) ? fopen(argv[2], outMode) : stdout;
  if (!out) {
    fprintf(stderr, "cannot write %s: %s\n", argv[2], strerror(errno));
    exit(1);
  }
  if (isEncode) {
    morseEncode(in, out);
  }
  else {
    morseDecode(in, out);
  }
  if (fclose(in) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", argv[1], strerror(errno));
    exit(1);
  }
  if (argc == 3 && fclose(out) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", argv[2], strerror(errno));
    exit(1);
  }
  return 0;
}
