#include <assert.h>
#include <limits.h> //for CHAR_BIT
#include <stdio.h>  //to allow you to print while debugging

#define TODO 0

// use wrapping overflow behavior to replace all occurrences of TODO
// below with suitable decimal literals to make all assertions true.
//
// *DO NOT MAKE ANY OTHER CHANGES*.
int main() {
  //assumptions made for this code
  assert(CHAR_BIT == 8);
  assert(sizeof(short) == 2);
  assert(sizeof(int) == 4);

  const unsigned char c1 = 240;
  const unsigned char c2 = c1 + TODO;
  assert(c2 == 2);

  const signed char c3 = 120;
  const signed char c4 = TODO;
  const signed char c5 = c3 + c4;
  assert(c4 > 0 && c5 == -126);

  const signed char c6 = TODO;
  const signed char c7 = -c6;
  assert(c6 == c7);

  const signed char c8 = -120;
  const signed char c9 = TODO;
  const signed char c10 = c8 + c9;
  assert(c9 < 0 && c10 == 127);

  const signed char c11 = -120;
  const signed char c12 = TODO;
  const signed char c13 = c11 + c12;
  assert(c12 < 0 && c13== 8);

  //C now allows ' within literals for readability
  const unsigned short s1 = 60'000;
  const unsigned short s2 = s1 + TODO;
  assert(s2 == 100);

  const signed short s3 = TODO;
  const signed short s4 = -s3;
  assert(s3 == s4);

  const signed short s5 = -30'000;
  const signed short s6 = TODO;
  const signed short s7 = s5 + s6;
  assert(s6 < 0 && s7 == 32'767);
}
