#include <limits.h>
#include <stdio.h>
#include <stdlib.h>


// The result of rotating val left by shift bits will be the word
// containing left_bits|right_bits. left_bits will simply be the
// result of shifting val to the left by shift bits (with shift 0's
// coming in from the right).  right_bits will be the rotation of the
// leftmost shift bits of val; this can be computed by shifting val
// RIGHT by (#-of-bits(val) - shift).
inline unsigned
rotate_left(unsigned val, int shift)
{
  unsigned left_bits = val << shift;
  //move shift leftmost bits to rightmost shift bits
  unsigned right_bits = val >> (sizeof(val)*CHAR_BIT - shift);
  return  left_bits | right_bits;
}


// The result of rotating val right by shift bits will be the word
// containing left_bits|right_bits. right_bits will simply be the
// result of shifting val to the right by shift bits (with shift 0's
// coming in from the left).  left_bits will be the rotation of the
// rightmost shift bits of val; this can be computed by shifting val
// LEFT by (#-of-bits(val) - shift).
inline unsigned
rotate_right(unsigned val, int shift)
{
  //TODO
  return 0;
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s HEX_VAL SHIFT\n", argv[0]);
    exit(1);
  }
  const unsigned val = strtol(argv[1], NULL, 16);
  const unsigned shift = atoi(argv[2]);
  printf("rotl(0x%x, %u) = 0x%x; rotr(0x%x, %u) = 0x%x\n",
         val, shift, rotate_left(val, shift),
         val, shift, rotate_right(val, shift));
}
