#ifndef BCD_H_
#define BCD_H_

#include <limits.h>
#include <stddef.h>

//.1.
/** Representation used for a BCD number: define based on BCD_BASE */
#ifndef BCD_BASE
  //default value for BCD_BASE
  #define BCD_BASE 2
#endif

//Depending on BCD_BASE, conditionally define:
//  Bcd:                 C type used to represent a BCD
//  BCD_FORMAT_MODIFIER: modifier used to printf() a BCD
//  SCANF_MODIFIER:      modifier used to scanf() a BCD
#if BCD_BASE == 0
  typedef unsigned char Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER "hh"
#elif BCD_BASE == 1
  typedef unsigned short Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER "h"
#elif BCD_BASE == 2
  typedef unsigned Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#elif BCD_BASE == 3
  typedef unsigned long Bcd;
  #define BCD_FORMAT_MODIFIER "l"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#elif BCD_BASE == 4
  typedef unsigned long long Bcd;
  #define BCD_FORMAT_MODIFIER "ll"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#else
  #error "invalid BCD_BASE value"
  //avoid subsequent compiler errors by providing definitions
  typedef unsigned long long Bcd;
  #define BCD_FORMAT_MODIFIER "ll"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#endif

//The above modifiers can be used to portably print and read Bcd values
//(note that in C, adjacent string literals are concatenated together).
//
// Bcd bcd;
// printf("0x%" BCD_FORMAT_MODIFIER "x", bcd); //output in hex
// printf("%" BCD_FORMAT_MODIFIER "u", bcd);   //output in decimal
// scanf("%" SCANF_MODIFIER "x", &bcd);        //read from stdin to bcd
//.2.

//since binary encoding of a particular size can represent all BCD
//encodings using the same size, the same C-type as Bcd can be used to
//represent binary representation of a Bcd number
typedef Bcd Binary;

enum {
  //# of bits in a BCD digit
  BCD_BITS = 4,

  //max # of digits within a Bcd
  MAX_BCD_DIGITS = sizeof(Bcd) * CHAR_BIT / BCD_BITS,

  //buffer size (including '\0') needed for a string representation of a Bcd
  BCD_BUF_SIZE = MAX_BCD_DIGITS + 1
};

//error codes returned by following API
typedef enum {
  NO_ERR,                  //no error
  BAD_VALUE_ERR,           //binary BCD value contains a digit > 9
  OVERFLOW_ERR             //an overflow was detected
} BcdError;



/** Set *bcd to BCD encoding of binary (which has normal binary
 * representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  Return OVERFLOW_ERR if binary is too big for the Bcd type,
 *  otherwise return NO_ERR.  Note that *bcd is undefined
 *  if the return value is not NO_ERR.
 */
BcdError binary_to_bcd(Binary value, Bcd *bcd);

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
BcdError bcd_to_binary(Bcd bcd, Binary *binary);

/** Set *bcd to BCD encoding of decimal number corresponding to string
 *  s.  Behavior undefined on overflow.  If p != NULL, sets *p to
 *  point to first non-digit char in s (as done for strtol()).
 *
 *  Return OVERFLOW_ERR if the number represented by s is too big for
 *  the Bcd type, otherwise return NO_ERR.  Note that *p and *bcd are
 *  undefined if the return value is not NO_ERR.
 */
BcdError str_to_bcd(const char *s, const char **p, Bcd *bcd);

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
BcdError bcd_to_str(Bcd bcd, char buf[], size_t buf_size, int *len);

/** Set *sum to the BCD representation of the sum of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *sum is undefined if the return value is not NO_ERR.
 */
BcdError bcd_add(Bcd n, Bcd m, Bcd *sum);

/** Set *sum to the BCD representation of the product of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *prod is undefined if the return value is not NO_ERR.
 */
BcdError bcd_multiply(Bcd n, Bcd m, Bcd *prod);


#endif //ifndef BCD_H_
