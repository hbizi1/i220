#ifndef INLINES_H_
#define INLINES_H_


#ifdef DO_TESTS
//hack to make static inline functions externally testable
#define static
#define inline
#endif

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
byte_bit_mask(unsigned bitIndex);

/** Given a power-of-2 powerOf2, return log2(powerOf2) */
static inline unsigned
get_log2_power_of_2(unsigned powerOf2);

/** Given a bitOffset return the bitIndex part of the bitOffset. */
static inline unsigned
get_bit_index(unsigned bitOffset);

/** Given a bitOffset return the byte offset part of the bitOffset */
static inline unsigned
get_byte_offset(unsigned bitOffset);

/** Return bit at offset bitOffset in array[]; i.e., return
 *  (bits(array))[bitOffset]
 */
static inline unsigned
get_bit_at_offset(const Byte array[], unsigned bitOffset);

/** Set bit selected by bitOffset in array to bit. */
static inline void
set_bit_at_offset(Byte array[], unsigned bitOffset, unsigned bit);

/** Set count bits in array[] starting at bitOffset to bit.  Return
 *  bit-offset one beyond last bit set.
 */
static inline unsigned
set_bits_at_offset(Byte array[], unsigned bitOffset,
                   unsigned bit, unsigned count);

/** Return count of run of identical bits starting at bitOffset
 *  in bytes[nBytes].
 *  Returns 0 when bitOffset outside bytes[nBytes].
 */
static inline unsigned
run_length(const Byte bytes[], unsigned nBytes, unsigned bitOffset);


#endif //ifndef INLINES_H_
