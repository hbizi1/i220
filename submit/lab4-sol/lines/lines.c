#include "memalloc.h"   //wrappers around malloc(), etc which abort on failure

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** A Point contains an (x, y) coordinate and a label */
typedef struct {
  int x, y;
  const char *label;
} Point;

/** A Line contains the (x0, y0), (x1, y1) coordinates of its end-points,
 *  a label and a link to a next Line (possibly NULL).
 */
// Note that the declaration contains both a `Line_` `struct` tag as well as
// a `Line` `typedef`.  The scope of each identifier starts *after* its
// declaration, hence declaring `succ` using `const Line *succ` would
// result in an error.  The declaration provided below avoids this problem
// by using `Line_` instead; however, `Line_` being a `struct` tag must
// always be specified as `struct Line_`.
//
// So the usual idiom is to use a struct tag within a structure
// definition to refer to the structure being defined and use a
// typedef for the entire structure outside the definition.`
typedef struct Line_ {
  int x0, y0, x1, y1;
  const char *label;
  const struct Line_ *succ;
} Line;

/** Create and return a dynamically allocated Line with end-points
 *  given by the coordinates of pt0 and pt1 and label which is
 *  built out of the labels of pt0 and pt1.
 *
 *  It is the responsibility of the caller to free the returned Line.
 */
static const Line *
add_line(const Line *last, const Point *pt0, const Point *pt1)
{
  const char *SEP = " --- ";
  const size_t label_len =
    strlen(pt0->label) + strlen(SEP) +  strlen(pt1->label);
  // get storage for label; we need an extra byte for the NUL
  // character terminating the string
  char *label = malloc_chk(label_len + 1); //+ 1 for NUL

  //snprintf is like printf(), but will "print" string into
  //array label[label_len + 1].  It will never overflow label[],
  //but will return the # of characters which would have been
  //printed (not including the terminating NUL).
  size_t n = snprintf(label, label_len + 1,
                      "%s%s%s", pt0->label, SEP, pt1->label);
  assert(n == label_len);

  //get storage for a Line
  Line *line = malloc_chk(sizeof(Line));

  //fill in components of Line
  line->x0 = pt0->x; line->y0 = pt0->y;
  line->x1 = pt1->x; line->y1 = pt1->y;
  line->label = label;
  line->succ = last;

  return line;
}


/** Free all storage occupied by list of lines */
static void
free_lines(const Line *lines)
{
  for (const Line *p = lines; p != NULL; p = p->succ) {
    free((void *)p->label);    //free label
    free((void*)p);            //free Line node
  }
}

static void
out_lines(FILE *out, const Line *lines)
{
  enum { W = 8 };  //printing width of a coordinate
  for (const Line *p = lines; p != NULL; p = p->succ) {
    fprintf(out, "(%*d, %*d) %20s (%*d, %*d)\n", W, p->x0, W, p->y0,
            p->label, W, p->x1, W, p->y1);
  }
}

static Point points[] = {
  { .x = 23452, .y = 36353, .label = "Grok" },
  { .x = 94539, .y = 43734, .label = "Nemo" },
  { .x = 84358, .y = 73757, .label = "Aladdin" },
  { .x = 63256, .y = 13631, .label = "Gandalf" },
  { .x = 93539, .y = 39563, .label = "Bilbo" },
  { .x = 11431, .y = 85487, .label = "Thorin" },
  { .x = 32341, .y = 72863, .label = "Thrain" },
  { .x = 43256, .y = 27642, .label = "Dain" },
  { .x = 22453, .y = 47323, .label = "Bilfur" },
  { .x = 84343, .y = 64585, .label = "Bombur" },
  { .x = 52342, .y = 67804, .label = "Nain" },
  { .x = 21373, .y = 75454, .label = "Smaug" },
  { .x = 73743, .y = 39476, .label = "Elrond" },
  { .x = 49755, .y = 74595, .label = "Thranduil" },
  { .x = 32523, .y = 23454, .label = "Gollum" },
  { .x = 23423, .y = 34424, .label = "Tolkien" },
};

int main() {
  const size_t n_points = sizeof(points)/sizeof(points[0]);
  assert(n_points % 2 == 0);
  const Line *lines = NULL;
  for (int i = 0; i < n_points; i += 2) {
    lines = add_line(lines, &points[i], &points[i + 1]);
  }
  out_lines(stdout, lines);
  free_lines(lines);
}
