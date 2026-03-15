#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

enum {
  BCD_MASK = (1 << BCD_BITS) - 1,
};

// TODO: define utility functions here


//remove this line when the project is complete
enum { TODO_ERR = -1 };

/** Set *bcd to BCD encoding of binary (which has normal binary
 * representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  Return OVERFLOW_ERR if binary is too big for the Bcd type,
 *  otherwise return NO_ERR. Note that *bcd is undefined if the return
 *  value is not NO_ERR.
 */
BcdError
binary_to_bcd(Binary value, Bcd *bcd)
{
  return TODO_ERR;
}

/** Set *binary to binary encoding of BCD value bcd.
 *
 *  Examples: bcd_to_binary(0x12) => 0xc;
 *            bcd_to_binary(0x255) => 0xff
 *
 *  Returns BAD_VALUE_ERR if bcd contains a bad BCD digit, otherwise
 *  return NO_ERR.  Cannot overflow since Binary can represent larger
 *  values than Bcd.  Note that *binary is undefined if the return
 *  value is not NO_ERR.
 */
BcdError
bcd_to_binary(Bcd bcd, Binary *binary)
{
  return TODO_ERR;
}

/** Set *bcd to BCD encoding of decimal number corresponding to string
 *  s.  Behavior undefined on overflow.  If p != NULL, sets *p to
 *  point to first non-digit char in s (as done for strtol()).
 *
 *  Return OVERFLOW_ERR if the number represented by s is too big for
 *  the Bcd type, otherwise return NO_ERR.  Note that *p and *bcd are
 *  undefined if the return value is not NO_ERR.
 */
BcdError
str_to_bcd(const char *s, const char **p, Bcd *bcd)
{
  return TODO_ERR;
}

/** Convert bcd to a NUL-terminated string in buf[] without any
 *  non-significant leading zeros.  Never write more than buf_size
 *  characters into buf.
 *
 *  Returns BAD_VALUE_ERR if bcd contains a BCD digit which is greater
 *  than 9, OVERFLOW_ERR if buf_size is not large enough for all
 *  digits and '\0' NUL, otherwise return NO_ERR.  Note that the
 *  contents of buf[] are undefined if the return value is
 *  not NO_ERR.
 *
 *  If return value is not BAD_VALUE_ERR and len != NULL, then set
 *  *len to the number of characters needed to write bcd (excluding
 *  the NUL character used to terminate strings).
 */
BcdError
bcd_to_str(Bcd bcd, char buf[], size_t buf_size, int *len)
{
  return TODO_ERR;
}

/** Set *sum to the BCD representation of the sum of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *sum is undefined if the return value is not NO_ERR.
 */
BcdError
bcd_add(Bcd n, Bcd m, Bcd *sum)
{
  return TODO_ERR;
}


/** Set *sum to the BCD representation of the product of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *prod is undefined if the return value is not NO_ERR.
 */
BcdError
bcd_multiply(Bcd n, Bcd m, Bcd *prod)
{
  return TODO_ERR;
}
