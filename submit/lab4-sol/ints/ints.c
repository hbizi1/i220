#include <assert.h>
#include <stdio.h>

int
main() {
  //verify exercise assumptions
  assert(sizeof(short) == 2);
  assert(sizeof(int) == 4);
  assert(sizeof(long) == 8);

  const unsigned short vals[] = {
    0x1234, 0x5678, 0x9abc, 0xdef0, 0x4321, 0x8765, 0xcba9, 0x0fed,
  };

  const size_t n_vals = sizeof(vals)/sizeof(vals[0]);

  //void * are generic pointers which contain an address of an unknown type.
  //they cannot be dereferenced or participate in pointer arithmetic.
  const void *base = vals;
  const void *end = &vals[n_vals];

  //void * pointers can be assigned to any other pointer types without
  //needing a cast.  Note that this is used with the void * return
  //value of malloc() and friends, where we can do something as simple
  //as Shape *p = malloc(sizeof(Shape));
  const unsigned short *shorts = base;
  const unsigned short *shorts_end = end;
  for (const unsigned short *p = shorts; p < shorts_end; p++) { //p loop-scoped

    //p - shorts is the difference of two pointers of type ptrdiff_t giving
    //the difference between the pointers in units of the pointed-to type.
    //%td is used for printing ptrdiff_t (since C99).
    printf("shorts[%td] = 0x%04x\n", p - shorts, *p);

  } //p is out of scope here
  printf("\n");

  //as above, but print out every third char in vals[] starting with the second
  const unsigned char *chars = base;
  const unsigned char *chars_end = end;
  for (const unsigned char *p = chars + 1; p < chars_end; p += 3) {
    printf("chars[%td] = 0x%02x\n", p - chars, *p);
  }
  printf("\n");


  //TODO: print out every second int in vals[]

  //TODO: print out all vals as long's.

}
