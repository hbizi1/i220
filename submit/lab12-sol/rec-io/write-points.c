#include "point2.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void
write_binary_points(FILE *out, const Point2 points[], size_t n_points)
{
  for (int i = 0; i < n_points; i++) {
    if (fwrite(&points[i], sizeof(Point2), 1, out) != 1) {
      fprintf(stderr, "cannot write binary point %d: %s\n",
              i, strerror(errno));
      exit(1);
    }
  }
}

static void
write_text_points(FILE *out, const Point2 points[], size_t n_points)
{
  for (int i = 0; i < n_points; i++) {
    fprintf(out, "%ld %ld\n", points[i].x, points[i].y);
  }
}


static const Point2 POINTS[] = {
  { 123'456'789, 987'654'321 },
  { 173, 432 },
};


int
main(int argc, const char *argv[])
{
  const char *text_ext = ".text";
  const size_t ext_len = strlen(text_ext);
  if (argc != 2) {
    fprintf(stderr, "usage: %s OUT_NAME\n"
            "output is text if OUT_NAME ends with \"%s\", else binary\n",
            argv[0], text_ext);
    exit(1);
  }
  const char *out_name = argv[1];
  const bool is_text =
    strlen(out_name) > ext_len &&
    strcmp(out_name + strlen(out_name) - ext_len, text_ext) == 0;
  const void (*write)(FILE *, const Point2 [], size_t) =
    is_text ? write_text_points : write_binary_points;

  const char *mode = (is_text) ? "w" : "wb";
  FILE *out = fopen(out_name, mode);
  if (!out) {
    fprintf(stderr, "cannot write %s: %s\n", out_name, strerror(errno));
    exit(1);
  }
  const size_t n_points = sizeof(POINTS)/sizeof(POINTS[0]);
  write(out, POINTS, n_points);
  if (fclose(out) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", out_name, strerror(errno));
    exit(1);
  }
}
