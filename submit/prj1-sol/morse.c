#include "morse.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  char c;
  const char *code;
} TextMorse;

//<https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg>
static const TextMorse char_codes[] = {
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "-.-." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', "-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },

  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },

  { '\0', ".-.-." }, //AR Prosign indicating End-of-message
                     //<https://en.wikipedia.org/wiki/Prosigns_for_Morse_code>
};


/** Return NUL-terminated morse code string (like "..--") for char
 *  c. Returns NULL if there is no code for c.
 */
static const char *
char_to_morse(Byte c) {
  for (int i = 0; i < sizeof(char_codes)/sizeof(char_codes[0]); i++) {
    if (char_codes[i].c == c) return char_codes[i].code;
  }
  return NULL;
}


/** Given a NUL-terminated morse code string (like "..--") for a
    single char, return the corresponding char. Returns < 0 if code is
    invalid.
 */
static int
morse_to_char(const char *code) {
  for (int i = 0; i < sizeof(char_codes)/sizeof(char_codes[0]); i++) {
    if (strcmp(char_codes[i].code, code) == 0) return char_codes[i].c;
  }
  return -1;
}

#include "inlines.h"

/** Given an array of Bytes, a bit index is the offset of a bit
 *  in the array (with MSB having offset 0).
 *
 *  Given a bytes[] array and some bitOffset, and assuming that
 *  BITS_PER_BYTE is 8, then (bitOffset >> 3) represents the index of
 *  the byte within bytes[] and (bitOffset & 0x7) gives the bit-index
 *  within the byte (MSB represented by bit-index 0) and .
 *
 *  For example, given array a[] = {0xB1, 0xC7} which is
 *  { 0b1011_0001, 0b1100_0111 } we have the following:
 *
 *     Bit-Offset   Value
 *        0           1
 *        1           0
 *        2           1
 *        3           1
 *        4           0
 *        5           0
 *        6           0
 *        7           1
 *        8           1
 *        9           1
 *       10           0
 *       11           0
 *       12           0
 *       13           1
 *       14           1
 *       15           1
 *
 */


/** Return a mask for a Byte with the single bit at bitIndex set to 1,
 *  all other bits set to 0.  Note that bitIndex == 0 represents the
 *  MSB, bitIndex == 1 represents the next significant bit and so on.
 *
 *  For example, if bitIndex == 0, then it should return 0x80 if
 *  BITS_PER_BYTE == 8 but 0x8000 if BITS_PER_BYTE == 16.  If bitIndex
 *  == 2, then it should return 0x20 if BITS_PER_BYTE == 8 but 0x2000
 *  if BITS_PER_BYTE == 16. Note that the code should work for any
 *  value of BITS_PER_BYTE.
 */
static inline unsigned
byte_bit_mask(unsigned bitIndex)
{
  //TODO
  return 0;
}

/** Given a power-of-2 powerOf2, return log2(powerOf2) */
static inline unsigned
get_log2_power_of_2(unsigned powerOf2)
{
  //TODO
  return 0;
}

/** Given a bitOffset return the bitIndex part of the bitOffset. */
static inline unsigned
get_bit_index(unsigned bitOffset)
{
  return bitOffset & (BITS_PER_BYTE - 1);
}

/** Given a bitOffset return the byte offset part of the bitOffset */
static inline unsigned
get_byte_offset(unsigned bitOffset)
{
  //TODO
  return 0;
}

/** Return bit at offset bitOffset in array[]; i.e., return
 *  (bits(array))[bitOffset]
 */
static inline unsigned
get_bit_at_offset(const Byte array[], unsigned bitOffset)
{
  //TODO
  return 0;
}

/** Set bit selected by bitOffset in array to bit. */
static inline void
set_bit_at_offset(Byte array[], unsigned bitOffset, unsigned bit)
{
  //TODO
}

/** Set count bits in array[] starting at bitOffset to bit.  Return
 *  bit-offset one beyond last bit set.
 */
static inline unsigned
set_bits_at_offset(Byte array[], unsigned bitOffset,
                   unsigned bit, unsigned count)
{
  //TODO
  return 0;
}

/** Return count of run of identical bits starting at bitOffset
 *  in bytes[nBytes].
 *  Returns 0 when bitOffset outside bytes[nBytes].
 */
static inline unsigned
run_length(const Byte bytes[], unsigned nBytes, unsigned bitOffset)
{
  //TODO
  return 0;
}



/** Convert text[nText] into a binary encoding of morse code in
 *  morse[].  It is assumed that array morse[] is initially all zero
 *  and is large enough to represent the morse code for all characters
 *  in text[].  The result in morse[] should be terminated by the
 *  morse prosign AR.  Any sequence of non-alphanumeric characters in
 *  text[] should be treated as a *single* inter-word space.  Leading
 *  non alphanumeric characters in text are ignored.
 *
 *  Returns count of number of bytes used within morse[].
 */
int
text_to_morse(const Byte text[], unsigned nText, Byte morse[])
{
  //TODO
  return 0;
}


/** Convert AR-prosign terminated binary Morse encoding in
 *  morse[nMorse] into text in text[].  It is assumed that array
 *  text[] is large enough to represent the decoding of the code in
 *  morse[].  Leading zero bits in morse[] are ignored.  Encodings
 *  representing word separators are output as a space ' ' character.
 *
 *  Returns count of number of bytes used within text[], < 0 on error.
 */
int
morse_to_text(const Byte morse[], unsigned nMorse, Byte text[])
{
  //TODO
  return 0;
}
