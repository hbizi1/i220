#include "bcd.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static inline const char *
skip_whitespace(const char *p) {
  while (isspace(*p)) p++;
  return p;
}

static int
is_error(BcdError err)
{
  switch (err) {
    case BAD_VALUE_ERR:
      fprintf(stderr, "bad BCD value > 9\n");
      return 1;
    case OVERFLOW_ERR:
      fprintf(stderr, "BCD overflow\n");
      return 1;
    default:
      break;
  }
  return 0;
}

int
main()
{
  enum { MAX_LINE = 80 };
  char line[MAX_LINE];
  printf("BCD_BASE == %d, sizeof(Bcd) == %zu\n", BCD_BASE, sizeof(Bcd));
  while (printf(">> ") && fflush(stdout) == 0 &&
         fgets(line, MAX_LINE, stdin) != NULL) {
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "line too long ... ignored\n");
      continue;
    }
    const char *p = skip_whitespace(line);
    Bcd result;
    BcdError err= str_to_bcd(p, &p, &result);
    if (is_error(err)) continue;
    p = skip_whitespace(p);
    if (*p == '+') {
      p = skip_whitespace(p + 1);
      Bcd operand2;
      err = str_to_bcd(p, &p, &operand2);
      if (is_error(err)) continue;
      err = bcd_add(result, operand2, &result);
      if (is_error(err)) continue;
    }
    else if (*p == '*') {
      p = skip_whitespace(p + 1);
      Bcd operand2;
      err = str_to_bcd(p, &p, &operand2);
      if (is_error(err)) continue;
      err = bcd_multiply(result, operand2, &result);
      if (is_error(err)) continue;
    }
    p = skip_whitespace(p);
    if (*p != '\0') {
      fprintf(stderr, "bad input %s", line);
    }
    else {
      char buf[BCD_BUF_SIZE];
      err = bcd_to_str(result, buf, BCD_BUF_SIZE, NULL);
      if (is_error(err)) continue;
      printf("%s (%" BCD_FORMAT_MODIFIER "u)\n", buf, result);
    }
  }
  if (ferror(stdin)) {
    fprintf(stderr, "i/o error on stdin: %s\n", strerror(errno));
  }
  return 0;
}
