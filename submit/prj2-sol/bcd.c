#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

enum {
  BCD_MASK = (1 << BCD_BITS) - 1,
};

static unsigned get_bcd_digit(Bcd bcd, int pos){
  return (bcd >> (pos * 4)) & 0xF;
}

static void set_bcd_digit(Bcd *bcd, int pos, unsigned digit){
  *bcd |= ((Bcd)digit << (pos * 4));
}

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
binary_to_bcd(Binary value, Bcd *bcd){
  Binary result = 0;
  int pos = 0;
  if (value == 0){
    *bcd=0;
    return NO_ERR;
  }
  while (value > 0){
    if (pos >= MAX_BCD_DIGITS){
      return OVERFLOW_ERR;
    }
    unsigned digit = value % 10;
    result |= ((Bcd)digit << (pos * 4));
    value /= 10;
    pos++;
  }
  *bcd = result;
  return NO_ERR;
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
bcd_to_binary(Bcd bcd, Binary *binary){
  Binary result = 0;
  for (int i = MAX_BCD_DIGITS - 1; i >= 0; i--) {
    unsigned digit = get_bcd_digit(bcd, i);
    if (digit > 9){
      return BAD_VALUE_ERR;
    }
    result = result * 10 + digit;
  }
  *binary = result;
  return NO_ERR;
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
str_to_bcd(const char *s, const char **p, Bcd *bcd){
  Bcd result = 0;
  int digits = 0;
  while (isdigit(*s)){
    if (digits >= MAX_BCD_DIGITS){
      return OVERFLOW_ERR;
    }
    unsigned digit = *s - '0';
    result <<= 4;
    result |= digit;
    s++;
    digits++;
  }
  if(p){
   *p = s;
  }
  *bcd = result;
  return NO_ERR;
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
bcd_to_str(Bcd bcd, char buf[], size_t buf_size, int *len){
  for (int i =0; i < MAX_BCD_DIGITS; i++){
    if (get_bcd_digit(bcd, i) > 9){
      return BAD_VALUE_ERR;
    }
  }
  int needed_char = snprintf(NULL, 0, "%llx", (unsigned long long)bcd);
  if (len != NULL){
    *len = needed_char;
  }
  if (buf_size < (size_t)(needed_char + 1)){
    return OVERFLOW_ERR;
  }
  snprintf(buf, buf_size, "%llx", (unsigned long long)bcd);
  return NO_ERR;
}

/** Set *sum to the BCD representation of the sum of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *sum is undefined if the return value is not NO_ERR.
 */
BcdError
bcd_add(Bcd n, Bcd m, Bcd *sum){
  Bcd result = 0;
  int carry = 0;
  for(int i = 0; i < MAX_BCD_DIGITS; i++){
    unsigned digitN = get_bcd_digit(n, i);
    unsigned digitM = get_bcd_digit(m, i);
    if (digitN > 9 || digitM > 9){
      return BAD_VALUE_ERR;
    }
    unsigned s = digitN + digitM + carry;
    if (s >= 10){
      carry = 1;
      s -= 10;
    }
    else {
      carry = 0;
    }
    set_bcd_digit(&result, i , s);
  }
  if (carry != 0){
    return OVERFLOW_ERR;
  }
  *sum = result;
  return NO_ERR;
}

/** Set *sum to the BCD representation of the product of BCD int's n and n.
 *
 *  Returns BAD_VALUE_ERR is n or m contains a BCD digit which is
 *  greater than 9, OVERFLOW_ERR on overflow, otherwise return NO_ERR.
 *  Note that *prod is undefined if the return value is not NO_ERR.
 */
static BcdError
bcd_multiply_digit(Bcd n, unsigned bcd_digit, Bcd *bcd){
  if (bcd_digit > 9){
    return BAD_VALUE_ERR;
  }
  Bcd result = 0;
  unsigned carry = 0;
  for (int i = 0; i < MAX_BCD_DIGITS; i++){
    unsigned digit = get_bcd_digit(n, i);
    if (digit > 9){
      return BAD_VALUE_ERR;
    }
    unsigned prod = digit * bcd_digit + carry;
    set_bcd_digit(&result, i, prod % 10);
    carry = prod / 10;
  }
  if (carry != 0){
    return OVERFLOW_ERR;
  }
  *bcd = result;
  return NO_ERR;
}

BcdError
bcd_multiply(Bcd n, Bcd m, Bcd *prod){
  Bcd result = 0;
  for (int i = 0; i < MAX_BCD_DIGITS; i++){
    unsigned digit = get_bcd_digit(m, i);
    if (digit > 9){
      return BAD_VALUE_ERR;
    }
    Bcd partial = 0;
    BcdError error = bcd_multiply_digit(n, digit, &partial);
    if (error != NO_ERR){
      return error;
    }
    if (i > 0){
      Bcd shift = 0;
      for (int j = MAX_BCD_DIGITS - 1; j >= i; j--){
        set_bcd_digit(&shift, j + i, get_bcd_digit(partial, j - i));
      }
      partial = shift;
    }
    Bcd temp;
    error = bcd_add(result, partial, &temp);
    if (error != NO_ERR){
      return error;
    }
    result = temp;
  }
  *prod = result;
  return NO_ERR;
}
