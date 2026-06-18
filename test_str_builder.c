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

// validate invariant helper
static void assert_valid_sb(StrBuilder *sb, const char *cstr) {
  assert(sb->data != NULL);
  assert(sb->cap >= 1);
  assert(sb->len + 1 <= sb->cap);
  assert(sb->data[sb->len] == '\0');
  assert(strlen(sb->data) == sb->len);
  assert(strcmp(cstr, sb->data) == 0);
}

// init -> the only way to initialize a valid object
static void test_init_null(void) {
  StrBuilder *sb = NULL;
  assert(!sb_init(sb));
}
// Note: with cap == 0, internal grow can only fail on
// allocator returning NULL, we don't test it.
// We test failure to grow with cap > 0 later indirectly
// via testing push.

static void test_init_ok(void) {
  StrBuilder sb;
  assert(sb_init(&sb));
  assert_valid_sb(&sb, "");
  assert(sb_free(&sb));
}

// free -> canonical uninitialized object
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

// push
static void test_push_cstr_null(void) {
  StrBuilder *sb = NULL;
  const char *cstr = "Hello";
  assert(!sb_push_cstr(sb, cstr));

  StrBuilder sb1;
  assert(sb_init(&sb1));
  assert(!sb_push_cstr(&sb1, NULL));

  assert(sb_free(&sb1));
}

static void test_push_cstr_empty(void) {
  StrBuilder sb;
  assert(sb_init(&sb));
  assert(sb_push_cstr(&sb, ""));
  assert_valid_sb(&sb, "");

  assert(sb_free(&sb));
}

static void test_push_cstr_many(void) {
  StrBuilder sb;
  assert(sb_init(&sb));

  const char *first = "Hello world! ";
  const char *second = "It's me 42. ";
  const char *last = "So long, and thanks for all the fish";
  static char result[100];
  int n = snprintf(result, 100, "%s%s%s", first, second, last);

  assert(sb_push_cstr(&sb, first));
  assert(sb_push_cstr(&sb, second));
  assert(sb_push_cstr(&sb, last));

  assert((size_t)n == sb.len);
  assert_valid_sb(&sb, result);

  assert(sb_free(&sb));
}

// append a string much larger than initial capacity
static void test_push_cstr_large(void) {
  StrBuilder sb;
  assert(sb_init(&sb));

  static char large[101];
  for (int i = 0; i < 100; i++) {
    large[i] = 'c';
  }
  large[100] = '\0';

  assert(sb_push_cstr(&sb, large));
  assert_valid_sb(&sb, large);
  // assert(sb.len == 100);

  assert(sb_free(&sb));
}

// We intentionally corrupt internal state to drive a hard-to-reach
// overflow/growth path.
static void test_push_cstr_inject_big_cap(void) {
  StrBuilder sb;
  assert(sb_init(&sb));
  const char *hello = "Hello!";

  assert(sb_push_cstr(&sb, hello));
  assert(strcmp(hello, sb.data) == 0);

  // make it fail on next push
  sb.cap = SIZE_MAX / 2 + 1;
  sb.len = sb.cap;

  assert(!sb_push_cstr(&sb, "World!"));
  assert(strcmp(hello, sb.data) == 0);

  assert(sb_free(&sb));
}

int main(void) {
  RUN_TEST(test_init_null);
  RUN_TEST(test_init_ok);
  RUN_TEST(test_free_null);
  RUN_TEST(test_free_after_init);
  RUN_TEST(test_push_cstr_null);
  RUN_TEST(test_push_cstr_empty);
  RUN_TEST(test_push_cstr_many);
  RUN_TEST(test_push_cstr_large);
  RUN_TEST(test_push_cstr_inject_big_cap);

  puts("All tests passed.");
  return EXIT_SUCCESS;
}
