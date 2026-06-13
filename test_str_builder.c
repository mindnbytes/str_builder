#include "str_builder.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TEST(fn)                                                           \
  do {                                                                         \
    printf("%s...\n", #fn);                                                    \
    fn();                                                                      \
  } while (0)

// init -> the only way to initialize a valid object
static void test_init_null(void) {
  StrBuilder *sb = NULL;
  assert(!sb_init(sb));
}
// Note: with cap == 0, internal grow can only fail on
// allocator returning NULL, we don't test it.
// We test failure to grow with cap > 0 later inderectly
// via testing push.

static void test_init_ok(void) {
  StrBuilder sb;
  assert(sb_init(&sb));
  assert(sb.len == 0 && sb.data[sb.len] == '\0');
  assert(sb_free(&sb));
}

// free -> cannonical uninitialized object
static void test_free_null(void) {
  StrBuilder *sb = NULL;
  assert(!sb_free(sb));
}

static void test_free_after_init(void) {
  StrBuilder sb;
  assert(sb_init(&sb));
  assert(sb_free(&sb));
  assert(sb.len == 0 && sb.cap == 0 && sb.data == NULL);
}

int main(void) {
  RUN_TEST(test_init_null);
  RUN_TEST(test_init_ok);
  RUN_TEST(test_free_null);
  RUN_TEST(test_free_after_init);

  puts("All tests passed.");
  return EXIT_SUCCESS;
}
