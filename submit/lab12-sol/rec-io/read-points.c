#include "point2.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** read n_points from stream in into array points[] */
static void
read_binary_points(FILE *in, Point2 points[], size_t n_points)
{
  //TODO: add implementation
}

static void
read_text_points(FILE *in, Point2 points[], size_t n_points)
{
  for (int i = 0; i < n_points; i++) {
    fscanf(in, "%ld %ld", &points[i].x, &points[i].y);
  }

}


int
main(int argc, const char *argv[])
{
  const char *text_ext = ".text";
  const size_t ext_len = strlen(text_ext);
  if (argc != 3) {
    fprintf(stderr, "usage: %s N_POINTS IN_NAME\n"
            "input is text if IN_NAME ends with \"%s\", else binary\n",
            argv[0], text_ext);
    exit(1);
  }
  const size_t n_points = atoi(argv[1]);
  const char *in_name = argv[2];
  const bool is_text =
    strlen(in_name) > ext_len &&
    strcmp(in_name + strlen(in_name) - ext_len, text_ext) == 0;
  const void (*read)(FILE *, Point2 [], size_t) =
    is_text ? read_text_points : read_binary_points;

  const char *mode = is_text ? "r" : "rb";
  FILE *in = fopen(in_name, mode);
  if (!in) {
    fprintf(stderr, "cannot read %s: %s\n", in_name, strerror(errno));
    exit(1);
  }
  Point2 points[n_points];
  read(in, points, n_points);
  if (fclose(in) != 0) {
    fprintf(stderr, "cannot close %s: %s\n", in_name, strerror(errno));
    exit(1);
  }

  FILE *out = stdout;
  for (int i = 0; i < n_points; i++) {
    fprintf(out, "%ld %ld\n", points[i].x, points[i].y);
  }

}
