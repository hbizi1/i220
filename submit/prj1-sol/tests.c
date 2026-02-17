#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "unit-test.h"

#include "morse.h"
#include "inlines.h"

/************************** byte_bit_mask() Tests ************************/

static void byte_bit_mask_tests(void) {
  const unsigned mask0 = 1 << (BITS_PER_BYTE - 1);
  UTEST_REL("byte_bit_mask MSB", mask0, ==, byte_bit_mask(0));

  const unsigned mask1 = 1 << (BITS_PER_BYTE - 2);
  UTEST_REL("byte_bit_mask second most significant bit",
        mask1, ==, byte_bit_mask(1));

  const unsigned lsb1_index = BITS_PER_BYTE - 2;
  UTEST_REL("second least significant bit",0x2, ==,  byte_bit_mask(lsb1_index));
  const unsigned lsb0_index = BITS_PER_BYTE - 1;
  UTEST_REL("least significant bit", 0x1, ==, byte_bit_mask(lsb0_index));
}

/************************ get_log2_power_of_2() Tests **********************/

static void get_log2_power_of_2_tests(void) {
  unsigned logs[] = { 0, 3, 4, 5, 8, 12 };
  for (int i = 0; i < sizeof(logs)/sizeof(logs[0]); i++) {
    const unsigned log = logs[i];
    const unsigned pow2 = 1u << log;
    char name[32];
    const int n = snprintf(name, sizeof(name), "get_log2_power_of_2(%u)", pow2);
    assert(n < sizeof(name));
    UTEST_REL(name, log, ==, get_log2_power_of_2(pow2));
  }
}

/************************** get_bit_index() Tests ************************/


static void get_bit_index_tests(void) {
  unsigned indexes[] = { 0x1a3f, 0x1a39, 0x1a3, 0x2872 };
  for (int i = 0; i < sizeof(indexes)/sizeof(indexes[0]); i++) {
    const unsigned index = indexes[i];
    const unsigned expected = index & (BYTE_SIZE == 2 ? 0x0f : 0x07);
    const unsigned actual = get_bit_index(index);
    char name[32];
    const int n = snprintf(name, sizeof(name), "get_bit_index(0x%04x)", index);
    assert(n < sizeof(name));
    UTEST_REL(name, expected, ==, actual);
  }
}


/************************** get_byte_offset() Tests **************************/

static void get_byte_offset_tests(void) {
  unsigned indexes[] = { 0x1a3f, 0x1a39, 0x1a3, 0x2872 };
  for (int i = 0; i < sizeof(indexes)/sizeof(indexes[0]); i++) {
    const unsigned index = indexes[i];
    const unsigned expected = index >> (BYTE_SIZE == 2 ? 4 : 3);
    const unsigned actual = get_byte_offset(index);
    char name[32];
    const int n =
      snprintf(name, sizeof(name), "get_byte_offset(0x%04x)", index);
    assert(n < sizeof(name));
    UTEST_REL(name, expected, ==, actual);
  }
}

/************************ get_bit_at_offset() Tests ***********************/

static void get_bit_at_offset_test(const Byte bytes[], unsigned offset,
                                   unsigned expected)
{
  const unsigned actual = get_bit_at_offset(bytes, offset);
  char name[32];
  const int n = snprintf(name, sizeof(name), "get_bit_at_offset(%d)", offset);
  assert(n < sizeof(name));
  UTEST_REL(name, expected, ==, actual);
}

static void get_bit_at_offset_tests(void) {
  const Byte bytes[] = { 0x1a, 0x23, 0x46 };

  //least significant but of 0x23
  const unsigned offset1 = BYTE_SIZE == 2 ? 31 : 15;
  get_bit_at_offset_test(bytes, offset1, 1);

  //lsb - 2 of 0x23
  const unsigned offset2 = BYTE_SIZE == 2 ? 29 : 13;
  get_bit_at_offset_test(bytes, offset2, 0);

  //most significant 1 in 0x46
  const unsigned offset3 = BYTE_SIZE == 2 ? 41 : 17;
  get_bit_at_offset_test(bytes, offset3, 1);

  //second most significant 1 in 0x46
  const unsigned offset4 = BYTE_SIZE == 2 ? 45 : 21;
  get_bit_at_offset_test(bytes, offset4, 1);

  //least significant bit in 0x46
  const unsigned offset5 = BYTE_SIZE == 2 ? 47 : 23;
  get_bit_at_offset_test(bytes, offset5, 0);

}


/************************ set_bit_at_offset() Tests ***********************/


static void
set_bit_at_offset_test(const Byte bytes[], size_t n_bytes,
                       unsigned offset, unsigned bit,
                       unsigned changed_byte_index, Byte changed_byte_val)
{
  Byte bytes1[n_bytes];
  memcpy(bytes1, bytes, n_bytes * sizeof(Byte));
  set_bit_at_offset(bytes1, offset, bit);
  for (int i = 0; i < n_bytes; i++) {
    char name[64];
    const int n = snprintf(name, sizeof(name),
                           "set_bit_at_offset(%d, %u): byte index %d",
                           offset, bit, i);
    assert(n < sizeof(name));
    Byte expected = (i == changed_byte_index) ? changed_byte_val : bytes[i];
    Byte actual = bytes1[i];
    UTEST_REL(name, expected, ==, actual);
  }
}

static void set_bit_at_offset_tests(void) {
  typedef struct {
    unsigned offset;
    unsigned changed_index;
    unsigned mask;
  } Test;
  const Test tests[] = { // use get_bit_at_offset() tests offsets and data
    { .offset = BYTE_SIZE == 2 ? 31 : 15, .changed_index = 1, .mask = 0x1 },
    { .offset = BYTE_SIZE == 2 ? 29 : 13, .changed_index = 1, .mask = 0x4 },
    { .offset = BYTE_SIZE == 2 ? 41 : 17, .changed_index = 2, .mask = 0x40 },
    { .offset = BYTE_SIZE == 2 ? 45 : 21, .changed_index = 2, .mask = 0x04 },
    { .offset = BYTE_SIZE == 2 ? 47 : 23, .changed_index = 2, .mask = 0x01 },
  };
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    const Test *t = &tests[i];
    const unsigned offset = t->offset;
    const unsigned changed_byte_index = t->changed_index;
    const unsigned mask = t->mask;
    for (unsigned value = 0; value < 2; value++) {
      const Byte bytes[] = { 0x1a, 0x23, 0x46 };
      const size_t n_bytes = sizeof(bytes)/sizeof(bytes[0]);
      const Byte b = bytes[t->changed_index];
      const Byte changed_byte_val =  (value == 0) ? (b & ~mask) : (b | mask);
      set_bit_at_offset_test(bytes, n_bytes, offset, value,
                             changed_byte_index, changed_byte_val);
    }
  }
}


/************************ set_bits_at_offset() Tests **********************/

static void set_bits_at_offset_tests(void) {
  enum { N_BYTES = 3 };
  Byte bytes[] = { 0x1a, 0x23, 0x46 };
  assert(sizeof(bytes)/sizeof(bytes[0]) == N_BYTES);

  typedef struct {
    unsigned offset;
    unsigned bit;
    unsigned count;
    Byte new_bytes[N_BYTES];
  } Test;
  const Test tests[] = {
    { .offset = BYTE_SIZE == 2 ? 28 : 12, .bit = 0, .count = 4,
      .new_bytes = { 0x1a, 0x20, 0x46 }
    },
    { .offset = BYTE_SIZE == 2 ? 28 : 12, .bit = 1, .count = 4,
      .new_bytes = { 0x1a, 0x2f, 0x46 }
    },
    { .offset = BYTE_SIZE == 2 ? 28 : 12, .bit = 0, .count = 5,
      .new_bytes = { 0x1a, 0x20, 0x46 }
    },
    { .offset = BYTE_SIZE == 2 ? 28 : 12, .bit = 1, .count = 5,
      .new_bytes = { 0x1a, 0x2f, BYTE_SIZE == 2 ? 0x8046 : 0xc6 }
    },
  };

  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    const Test *t = &tests[i];
    Byte bytes1[N_BYTES];
    memcpy(bytes1, bytes, N_BYTES*sizeof(Byte));
    unsigned next_offset =
      set_bits_at_offset(bytes1, t->offset, t->bit, t->count);
    char name[64];
    const int n = snprintf(name, sizeof(name),
                           "set_bits_at_offset(%d, %u, %u)",
                           t->offset, t->bit, t->count);
    assert(n < sizeof(name));
    UTEST_REL(name, t->offset + t->count, ==, next_offset);
    for (int i = 0; i < N_BYTES; i++) {
      char name[64];
      const int n = snprintf(name, sizeof(name),
                             "set_bits_at_offset(%d, %u, %u): byte index %d",
                             t->offset, t->bit, t->count, i);
      assert(n < sizeof(name));
      UTEST_REL(name, t->new_bytes[i], ==, bytes1[i]);
    }
  }

}


/*************************** run_length() Tests *************************/

static void run_length_tests(void) {
  const int is2 = (BYTE_SIZE == 2);
  const Byte bytes[] = { 0x1d,  is2 ? 0x3003 : 0x33, is2 ? 0xfc00 : 0xfc };
  const size_t n_bytes = sizeof(bytes)/sizeof(bytes[0]);
  typedef struct {
    unsigned offset;
    unsigned run_len;
  } Test;
  Test tests[] = {
    { .offset = is2 ? 11 : 3, .run_len = 3 },  //init 3 1s in 0x1d 0001_1101
    { .offset = is2 ? 15 : 7, .run_len = 1 },  //0s from LSB 0x1d to 0x30
    { .offset = is2 ? 30 : 14, .run_len = 8 }, //1s starting 2nd last to last
    { .offset = is2 ? 47 : 23, .run_len = 1 }, //last bit
    { .offset = is2 ? 48 : 24, .run_len = 0 }, //just outside bytes[]
    { .offset = is2 ? 60 : 80, .run_len = 0 }, //well outside bytes[]
  };
  for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    const Test *t = &tests[i];
    const unsigned actual = run_length(bytes, n_bytes, t->offset);
    char name[32];
    const int n = snprintf(name, sizeof(name), "run_length(%u)", t->offset);
    assert(n < sizeof(name));
    UTEST_REL(name, t->run_len, ==, actual);
  }
}


/********************* Morse Encode / Decode Tests *********************/

const Byte SOS[] = { 'S', 'O', 'S' };
const unsigned nSOS = sizeof(SOS)/sizeof(SOS[0]);

/*
1 0 1 0 1 0 0 0   1 1 1 0 1 1 1 0   1 1 1 0 0 0 1 0    1 0 1 0 0 0 1 0
  .   .   .             -       -         -       .      .   .       .
              S                               O                  S
0xa8              0xee              0xe2               0xa2


1 1 1 0 1 0 1 1   1 0 1 0 0 0 0 0
      -   .         -   .
        A                 R
0xeb              0xa0
 */

#if BYTE_SIZE == 2
static Byte SOSBin[] = { 0xa8ee, 0xe2a2, 0xeba0, };
#else
static Byte SOSBin[] = { 0xa8, 0xee, 0xe2, 0xa2, 0xeb, 0xa0, };
#endif

static unsigned nSOSBin = sizeof(SOSBin)/sizeof(SOSBin[0]);


static void text_to_morse_sos_test(void) {
  Byte bytes[] = { [9] = 0 }; //force all 0's

  const int actual = text_to_morse(SOS, nSOS, bytes);
  UTEST_REL("text_to_morse_sos return", nSOSBin, ==, actual);
  for (int i = 0; i < nSOSBin; i++) {
    char name[32];
    const int n = snprintf(name, sizeof(name),
                           "text_to_morse_sos byte %u", i);
    assert(n < sizeof(name));
    UTEST_REL(name, SOSBin[i], ==, bytes[i]);
  }

}

static void morse_to_text_sos_test(void) {
  Byte text[] = { [9] = 0 }; //force all 0's

  const int actual = morse_to_text(SOSBin, nSOSBin, text);
  UTEST_REL("morse_to_text_sos return", nSOS, ==, actual);
  for (int i = 0; i < nSOS; i++) {
    char name[32];
    const int n = snprintf(name, sizeof(name),
                           "morse_to_text_sos byte %u", i);
    assert(n < sizeof(name));
    UTEST_REL(name, SOS[i], ==, text[i]);
  }
}

/*************************** Main Test Function ************************/

int is_verbose_unit_test = 1;
int n_fails_unit_test = 0;

int
main(void)
{
  assert(sizeof(unsigned short) == 2);

  is_verbose_unit_test = 1;

  // byte_bit_mask_tests();
  // get_log2_power_of_2_tests();
  // get_bit_index_tests();
  // get_byte_offset_tests();
  // get_bit_at_offset_tests();
  // set_bit_at_offset_tests();
  // set_bits_at_offset_tests();
  // run_length_tests();

  // text_to_morse_sos_test();
  // morse_to_text_sos_test();

  return n_fails_unit_test;
}
