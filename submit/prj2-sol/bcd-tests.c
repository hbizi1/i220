#include "bcd.h"

#include "unit-test.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *descr;
  Binary binary;
  Bcd bcd;
  char str[BCD_BUF_SIZE + 1];  //extra char to allow for bcd overflow
} BcdInfo;

typedef struct Data {
  BcdInfo zero;           //0
  BcdInfo consecutive;    //consecutive digits 123...89012...
  BcdInfo max;            //all 9's
  BcdInfo max1;           //max - 1
  BcdInfo max4;           //max - 4
  BcdInfo maxHalfTrunc;   //truncated max/2: 499...99
  BcdInfo maxHalfRound;   //500...00
  /*
  BcdInfo overflow;       //max + 1
  BcdInfo badVal;         //9999...99a
  */
} Data;

enum { MAX_TEST_NAME = 128 };

static Data DATA;

static const BcdInfo *TESTS[] = {
    &DATA.zero, &DATA.consecutive, &DATA.max, &DATA.max1, &DATA.max4,
    &DATA.maxHalfTrunc, &DATA.maxHalfRound,  //&DATA.overflow, &DATA.badVal,
};

/** If digit >= 0, return a binary of MAX_BCD_DIGITS indentical digit's,
 *  else return a binary consisting of MAX_BCD_DIGITS consecutive digit's
 *  starting with digit.
 */
static Binary
make_binary(int digit) {
  Binary val = 0;
  int d = 1;
  for (int i = 0; i < MAX_BCD_DIGITS; i++) {
    val = val*10 + (digit < 0 ? d : digit);
    d = (d + 1)%10;
  }
  return val;
}

/** If digit >= 0, return a bcd of MAX_BCD_DIGITS indentical digit's,
 *  else return a bcd consisting of MAX_BCD_DIGITS consecutive digit's
 *  starting with digit.
 */
__attribute__((unused))
static Bcd
make_bcd(int digit) {
  Bcd val = 0;
  int d = 1;
  for (int i = 0; i < MAX_BCD_DIGITS; i++) {
    val = val*16 + (digit < 0 ? d : digit);
    d = (d + 1)%10;
  }
  return val;
}

/** If digit >= 0, fill in str with MAX_BCD_DIGITS indentical digit's,
 *  else fill in str with MAX_BCD_DIGITS consecutive digit's
 *  starting with digit.  It is assumed that str has space for
 *  MAX_BCD_DIGITS plus the terminating '\0';
 */
__attribute__((unused))
static void
make_bcd_str(int digit, char str[]) {
  int d = 1;
  for (int i = 0; i < MAX_BCD_DIGITS; i++) {
    str[i] = (digit < 0 ? d : digit);
    d = (d + 1)%10;
  }
  str[MAX_BCD_DIGITS] = '\0';
}

static void
fill_bcd_info(const char *descr, Binary binary, BcdInfo *info)
{
  info->descr = descr;
  info->binary = binary;
  int n = snprintf(info->str, sizeof(info->str),
                   "%" BCD_FORMAT_MODIFIER "u", binary);
  assert(n < sizeof(info->str));
  sscanf(info->str, "%" SCANF_MODIFIER "x", &info->bcd);
}

__attribute__((unused))
static const char *
err_str(BcdError err) {
  switch (err) {
    case NO_ERR:
      return "NO_ERR";
    case BAD_VALUE_ERR:
      return "BAD_VALUE_ERR";
    case OVERFLOW_ERR:
      return "OVERFLOW_ERR";
    default:
      fprintf(stderr, "error value %d not handled\n", err);
      exit(1);
  }
  return NULL;
}


__attribute__((unused))
static void
dumpTestData(const Data *const p, FILE *out)
{
  fprintf(out, "*** BEGIN_TEST DATA for BCD_BASE = %d\n", BCD_BASE);
  fprintf(out, "sizeof(Bcd) = %zu; BCD_MAX_DIGITS = %d\n",
          sizeof(Bcd), MAX_BCD_DIGITS);
  const BcdInfo *infos[] = {
    &p->zero, &p->consecutive, &p->max, &p->max1, &p->max4,
    &p->maxHalfTrunc, &p->maxHalfRound, //&p->overflow, &p->badVal,
  };
  for (unsigned i = 0; i < sizeof(infos)/sizeof(infos[0]); i++) {
    const BcdInfo *infoP = infos[i];
    fprintf(out, "%s: ", infoP->descr);
    fprintf(out, "str: \"%s\": ", infoP->str);
    fprintf(out, "binary: %" BCD_FORMAT_MODIFIER "u; ", infoP->binary);
    fprintf(out, "bcd: 0x%" BCD_FORMAT_MODIFIER "x\n", infoP->bcd);
  }

  fprintf(out, "*** END_TEST DATA for BCD_BASE = %d\n", BCD_BASE);
}

//#define DUMP_TEST_DATA
static void
initTestData(Data *data)
{
  fill_bcd_info("zero", make_binary(0), &DATA.zero);
  fill_bcd_info("consecutive", make_binary(-1), &DATA.consecutive);
  fill_bcd_info("max", make_binary(9), &DATA.max);
  fill_bcd_info("max - 2", make_binary(9) - 1, &DATA.max1);
  fill_bcd_info("max - 4", make_binary(9) - 4, &DATA.max4);
  fill_bcd_info("maxHalfTrunc", make_binary(9)/2, &DATA.maxHalfTrunc);
  fill_bcd_info("maxHalfRound", make_binary(9)/2 + 1, &DATA.maxHalfRound);
  /*
  fill_bcd_info("overflow", make_binary(9) + 1, &DATA.overflow);
  fill_bcd_info("badVal", make_binary(9), &DATA.badVal);
  DATA.badVal.bcd = (DATA.max.bcd & ~0xf) | 0xa;
  DATA.badVal.binary += 1;
  DATA.badVal.str[BCD_BUF_SIZE - 2] = 'a';
  */

  #ifdef DUMP_TEST_DATA
  dumpTestData(&DATA, stderr);
  #endif
}

/************************* binary_to_bcd() Tests ***********************/

static void
binary_to_bcd_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    const BcdInfo *t = TESTS[i];
    Bcd bcd;
    BcdError err = binary_to_bcd(t->binary, &bcd);
    int n = snprintf(name, sizeof(name), "%s: binary_to_bcd(%lld) value",
                     t->descr, (long long)t->binary);
    assert(n < sizeof(name));
    UTEST_REL(name, t->bcd, ==, bcd);
    n = snprintf(name, sizeof(name), "%s NO_ERR: binary_to_bcd(%lld)",
                 t->descr, (long long)t->binary);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  { //overflow error
    Binary binary = make_binary(9) + 1;
    Bcd bcd;
    BcdError err = binary_to_bcd(binary, &bcd);
    int n = snprintf(name, sizeof(name),
                     "OVERFLOW_ERR: binary_to_bcd(%lld)",
                     (long long)binary);
    assert(n < sizeof(name));
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
  }
}


/************************* bcd_to_binary() Tests ***********************/

static void
bcd_to_binary_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    const BcdInfo *t = TESTS[i];
    Binary binary;
    BcdError err = bcd_to_binary(t->bcd, &binary);
    int n = snprintf(name, sizeof(name), "%s: bcd_to_binary(0x%llx) value",
                     t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, t->binary, ==, binary);
    n = snprintf(name, sizeof(name), "%s NO_ERR: bcd_to_binary(0x%llx)",
                 t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  { //bad value error
    Bcd bcd = (make_binary(9) & ~0xf) | 0xa;
    Binary binary;
    BcdError err = bcd_to_binary(bcd, &binary);
    int n = snprintf(name, sizeof(name), "BAD_VALUE_ERR: bcd_to_binary(0x%llx)",
                     (long long)bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
}

/************************** str_to_bcd() Tests *************************/

static void
str_to_bcd_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    const BcdInfo *t = TESTS[i];
    Bcd bcd;
    BcdError err = str_to_bcd(t->str, NULL, &bcd);
    int n = snprintf(name, sizeof(name), "%s: str_to_bcd(\"%s\") value",
                     t->descr, t->str);
    assert(n < sizeof(name));
    UTEST_REL(name, t->bcd, ==, bcd);
    n = snprintf(name, sizeof(name), "%s NO_ERR: binary_to_bcd(\"%s\")",
                 t->descr, t->str);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
    const char *p;
    str_to_bcd(t->str, &p, &bcd);
    n = snprintf(name, sizeof(name),
                     "%s: str_to_bcd(\"%s\") left-over char '\\0'",
                     t->descr, t->str);
    assert(n < sizeof(name));
    UTEST_REL(name, '\0', ==, *p);
  }
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    //check for leftover char
    const BcdInfo *t = TESTS[i];
    Bcd bcd;
    char str[BCD_BUF_SIZE + 1];
    char c = 'a' + i;
    int n1 = snprintf(str, sizeof(str), "%s%c", t->str, c);
    assert(n1 < sizeof(str));
    const char *p;
    str_to_bcd(str, &p, &bcd);
    int n = snprintf(name, sizeof(name),
                     "%s: str_to_bcd(\"%s\") left-over char '%c'",
                     t->descr, t->str, c);
    assert(n < sizeof(name));
    UTEST_REL(name, c, ==, *p);
  }
  { //overflow error
    Binary binary = make_binary(1);
    char str[BCD_BUF_SIZE + 1];
    int n1 = snprintf(str, sizeof(str), "1%lld", (long long)binary);
    assert(n1 < sizeof(str));
    Bcd bcd;
    BcdError err = str_to_bcd(str, NULL, &bcd);
    int n2 = snprintf(name, sizeof(name), "OVERFLOW_ERR: str_to_bcd(\"%s\")",
                     str);
    assert(n2 < sizeof(name));
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
  }
}


/************************** bcd_to_str() Tests *************************/

static void
bcd_to_str_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    const BcdInfo *t = TESTS[i];
    char actual[BCD_BUF_SIZE];
    BcdError err = bcd_to_str(t->bcd, actual, sizeof(actual), NULL);
    int n = snprintf(name, sizeof(name), "%s: bcd_to_str(0x%llx) value",
                     t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    const char *expected = t->str;
    UTEST_COND(name, strcmp(expected, actual) == 0,
               "actual value \"%s\" != expected value \"%s\"\n",
               actual, expected);
    n = snprintf(name, sizeof(name), "%s NO_ERR: bcd_to_str(0x%llx)",
                 t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  for (int i = 0; i < sizeof(TESTS)/sizeof(TESTS[0]); i++) {
    //force overflow errors
    const BcdInfo *t = TESTS[i];
    const size_t len = strlen(t->str);
    char actual[len];  //no space for NUL
    int size;
    BcdError err = bcd_to_str(t->bcd, actual, sizeof(actual), &size);
    int n = snprintf(name, sizeof(name),
                     "%s OVERFLOW_ERR: bcd_to_str(0x%llx) value",
                     t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
    n = snprintf(name, sizeof(name), "%s size: bcd_to_str(0x%llx)",
                 t->descr, (long long)t->bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, len, ==, size);
  }
  { //bad value error
    Bcd bcd = (make_binary(9) & ~0xf) | 0xa;
    char str[BCD_BUF_SIZE];
    BcdError err = bcd_to_str(bcd, str, sizeof(str), NULL);
    int n = snprintf(name, sizeof(name), "BAD_VALUE_ERR: bcd_to_str(0x%llx)",
                     (long long)bcd);
    assert(n < sizeof(name));
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
}

/**************************** bcd_add() Tests **************************/

static void
bcd_add_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  Bcd sum;
  Bcd expected;
  int n;
  BcdError err;
  if (true) { //consecutive + 1
    const BcdInfo *t = &DATA.consecutive;
    err = binary_to_bcd(t->binary + 1, &expected);
    assert(err == NO_ERR);

    n = snprintf(name, sizeof(name),
                 "inc-consecutive: bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", t->bcd, (Bcd)0x1);
    assert(n < sizeof(name));
    err = bcd_add(t->bcd, 0x1, &sum);
    UTEST_REL(name, expected, ==, sum);

    n = snprintf(name, sizeof(name),
                 "error bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", t->bcd, (Bcd)0x1);
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //maxHalfTrunc + maxHalfRound
    Bcd arg1 = DATA.maxHalfTrunc.bcd;
    Bcd arg2 = DATA.maxHalfRound.bcd;
    n = snprintf(name, sizeof(name),
                 "maxHalfTrunc + maxHalfRound: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_add(arg1, arg2, &sum);

    Bcd expected = DATA.max.bcd;
    UTEST_REL(name, expected, ==, sum);

    n = snprintf(name, sizeof(name),
                 "error, bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //maxHalfTrunc + maxHalfTrunc
    Bcd arg1 = DATA.maxHalfTrunc.bcd;
    Bcd arg2 = DATA.maxHalfTrunc.bcd;
    n = snprintf(name, sizeof(name),
                 "check-carry: maxHalfTrunc + maxHalfTrunc: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_add(arg1, arg2, &sum);

    Bcd expected = DATA.max1.bcd;
    UTEST_REL(name, expected, ==, sum);

    n = snprintf(name, sizeof(name),
                 "error, bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //badVal + maxHalfTrunc
    Bcd bad_val = (DATA.maxHalfTrunc.bcd & ~0xf0) | 0xa0;
    Bcd arg2 = DATA.maxHalfTrunc.bcd;
    n = snprintf(name, sizeof(name),
                 "bad-val + maxHalfTrunc: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", bad_val, arg2);
    assert(n < sizeof(name));
    err = bcd_add(bad_val, arg2, &sum);
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
  if (true) { //max4 + badVal
    Bcd arg1 = DATA.max4.bcd;
    Bcd bad_val = (DATA.max4.bcd & ~0xf0) | 0xa0;
    n = snprintf(name, sizeof(name),
                 "max4 + bad-val: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, bad_val);
    assert(n < sizeof(name));
    err = bcd_add(arg1, bad_val, &sum);
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
  if (true) { //max4 + 5
    Bcd arg1 = DATA.max4.bcd;
    Bcd arg2 = 0x5;
    n = snprintf(name, sizeof(name),
                 "max4 + 5: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_add(arg1, arg2, &sum);
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
  }
  if (true) { //2 + max1
    Bcd arg1 = 0x2;
    Bcd arg2 = DATA.max1.bcd;
    n = snprintf(name, sizeof(name),
                 "2 + max1: "
                 "bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_add(arg1, arg2, &sum);
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
  }
}

/************************** bcd_multiply() Tests ***********************/

static void
bcd_multiply_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  Bcd prod;
  Bcd expected;
  int n;
  BcdError err;
  if (true) { //consecutive * 2
    Bcd arg1 = DATA.consecutive.bcd;
    Bcd arg2 = 0x2;
    err = binary_to_bcd(DATA.consecutive.binary*2, &expected);
    assert(err == NO_ERR);

    n = snprintf(name, sizeof(name),
                 "consecutive * 2: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_multiply(arg1, arg2, &prod);
    UTEST_REL(name, expected, ==, prod);

    n = snprintf(name, sizeof(name),
                 "error, bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //maxHalfTrunc * 0x2
    Bcd arg1 = DATA.maxHalfTrunc.bcd;
    Bcd arg2 = 0x2;
    Binary arg_bin = DATA.maxHalfTrunc.binary;
    err = binary_to_bcd(arg_bin*2, &expected);
    assert(err == NO_ERR);

    n = snprintf(name, sizeof(name),
                 "maxHalfTrunc * 0x2: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_multiply(arg1, arg2, &prod);
    UTEST_REL(name, expected, ==, prod);

    n = snprintf(name, sizeof(name),
                 "error, bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //0x9 * 0x9
    Bcd arg1 = 0x9;
    Bcd arg2 = 0x9;
    expected = 0x81;

    n = snprintf(name, sizeof(name),
                 "0x9 * 0x9: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_multiply(arg1, arg2, &prod);
    UTEST_REL(name, expected, ==, prod);

    n = snprintf(name, sizeof(name),
                 "error, bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err);
  }
  if (true) { //badVal * maxHalfTrunc
    Bcd bad_val = (DATA.maxHalfTrunc.bcd & ~0xf0) | 0xa0;
    Bcd arg2 = DATA.maxHalfTrunc.bcd;
    n = snprintf(name, sizeof(name),
                 "bad-val * maxHalfTrunc: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", bad_val, arg2);
    assert(n < sizeof(name));
    err = bcd_multiply(bad_val, arg2, &prod);
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
  if (true) { //0x1 * badVal
    Bcd arg1 = 0x1;
    Bcd bad_val = (DATA.max4.bcd & ~0xf0) | 0xa0;
    n = snprintf(name, sizeof(name),
                 "0x1 * badVal: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, bad_val);
    assert(n < sizeof(name));
    err = bcd_multiply(arg1, bad_val, &prod);
    UTEST_REL(name, BAD_VALUE_ERR, ==, err);
  }
  if (true) { //maxHalfRound * 0x2
    Bcd arg1 = DATA.maxHalfRound.bcd;
    Bcd arg2 = 0x2;

    n = snprintf(name, sizeof(name),
                 "OVERFLOW_ERR: maxHalfRound * 0x2: "
                 "bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    err = bcd_multiply(arg1, arg2, &prod);
    UTEST_REL(name, OVERFLOW_ERR, ==, err);
  }
}

/***************************** Multiop Tests ***************************/

static void
bcd_multiop_tests(void)
{
  return;  //TODO: remove this line to activate tests
  char name[MAX_TEST_NAME];
  Bcd sum;
  Bcd prod;
  int n;
  if (true) { //39 * 2 + 3 == 81
    Bcd arg1 = 0x39;
    Bcd arg2 = 0x2;
    Bcd arg3 = 0x3;
    Bcd expected = 0x81;
    BcdError err1 = bcd_multiply(arg1, arg2, &prod);
    n = snprintf(name, sizeof(name),
                 "39 * 2: "
                 "error bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err1);

    BcdError err2 = bcd_add(prod, arg3, &sum);
    n = snprintf(name, sizeof(name),
                 "adding 3: "
                 "error bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", prod, arg3);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err2);

    n = snprintf(name, sizeof(name), "39 * 2 + 3");
    assert(n < sizeof(name));
    UTEST_REL(name, expected, ==, sum);

  }
  if (true) { //(29 + 2) * 3 == 93
    Bcd arg1 = 0x29;
    Bcd arg2 = 0x2;
    Bcd arg3 = 0x3;
    Bcd expected = 0x93;
    BcdError err1 = bcd_add(arg1, arg2, &sum);
    n = snprintf(name, sizeof(name),
                 "29 + 2: "
                 "error bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err1);

    BcdError err2 = bcd_multiply(sum, arg3, &prod);
    n = snprintf(name, sizeof(name),
                 "multiplying by 3: "
                 "error bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", sum, arg3);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err2);

    n = snprintf(name, sizeof(name), "(29 + 2) * 3");
    assert(n < sizeof(name));
    UTEST_REL(name, expected, ==, prod);
  }
  if (true) { //overflow on maxHalfTrunc * 2 + 3
    Bcd arg1 = DATA.maxHalfTrunc.bcd;
    Bcd arg2 = 0x2;
    Bcd arg3 = 0x3;
    BcdError err1 = bcd_multiply(arg1, arg2, &prod);
    n = snprintf(name, sizeof(name),
                 "maxHalfTrunc * 2: "
                 "error bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
                 BCD_FORMAT_MODIFIER "x)", arg1, arg2);
    assert(n < sizeof(name));
    UTEST_REL(name, NO_ERR, ==, err1);

    BcdError err2 = bcd_add(prod, arg3, &sum);
    n = snprintf(name, sizeof(name), "overflow on maxHalfTrunc*2 + 3");
    assert(n < sizeof(name));
    UTEST_REL(name, OVERFLOW_ERR, ==, err2);
  }
}

/************************** Testing main() *****************************/

int is_verbose_unit_test = 1;
int n_fails_unit_test = 0;

int
main(void)
{
  initTestData(&DATA);

  binary_to_bcd_tests();
  bcd_to_binary_tests();
  str_to_bcd_tests();
  bcd_to_str_tests();
  bcd_add_tests();
  bcd_multiply_tests();
  bcd_multiop_tests();

  return n_fails_unit_test;
}
