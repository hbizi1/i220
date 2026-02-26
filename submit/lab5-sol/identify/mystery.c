#include "mystery.h"

/** Return v with value masked off to n-bits for some fixed n. */
unsigned int
mystery(unsigned int v)
{
  enum { MASK = 0x7fffff };
  return v & MASK;
}
