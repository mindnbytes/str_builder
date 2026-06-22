#include "str_builder.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Private helper, grow internal array
// return false if it fails, true otherwise
static bool sb_grow(StrBuilder *sb) {
  // check next cap overflow
  if (sb->cap > SIZE_MAX / 2) // technically, extra / sizeof(char), but that's 1
    return false;
  size_t new_cap = sb->cap == 0 ? 16 : sb->cap * 2;

  char *new_data = realloc(sb->data, new_cap * sizeof(sb->data[0]));
  if (new_data == NULL)
    return false;

  sb->data = new_data;
  sb->cap = new_cap;

  return true;
}
// Private helper for push operations.
// Trust caller that pointed to memory has at least n chars/bytes to read.
// On failure, the logical string content and len are unchanged.
// The builder remains valid. Capacity may have changed.
// Writes n chars/bytes from src to internal buffer and
// returns true. Preserves valid string builder invariants.
// Private helper for text fragments only; embedded '\0' is caller misuse!
static bool sb_push_n(StrBuilder *sb, const char *src, size_t n) {
  // no-op
  if (n == 0)
    return true;
  // check for potential len overflow
  if (n > SIZE_MAX - sb->len - 1)
    return false;
  // check if src is internal and compute offset
  bool src_is_internal = false;
  size_t src_offset = 0;
  for (size_t i = 0; i <= sb->len; i++) {
    if (src == sb->data + i) {
      src_is_internal = true;
      src_offset = i;
      break;
    }
  }
  // grow
  size_t needed = sb->len + n + 1;
  while (needed > sb->cap) {
    if (!sb_grow(sb))
      return false;
    // adjust src if internal
    if (src_is_internal) {
      src = sb->data + src_offset;
    }
  }
  // copy
  char *dst = sb->data + sb->len;
  memcpy(dst, src, n);
  // correct state
  sb->len += n;
  sb->data[sb->len] = '\0';
  return true;
}

// Must call to initialize a valid object
// don't call on the pointer to already initialized
// object!
// The object is valid only when true is returned
// Returns false on failures:
// - sb is NULL
// - failing to allocate memory
// Note: calling init on already initialized and valid
// object will lead to memory leak
bool sb_init(StrBuilder *sb) {
  if (sb == NULL)
    return false;
  // ensure clean state
  sb->cap = 0;
  sb->len = 0;
  sb->data = NULL;
  // allocate
  if (!sb_grow(sb))
    return false;
  // valid c str invariant
  sb->data[sb->len] = '\0';
  return true;
}

// Must call to free internal memory
// Object isn't valid after free.
// Returns false if sb is NULL.
// Otherwise releases the internal buffer and returns true.
// Calling sb_free on an uninitialized StrBuilder object is undefined behavior,
// because sb->data may contain an indeterminate pointer value.
bool sb_free(StrBuilder *sb) {
  if (sb == NULL)
    return false;
  free(sb->data);
  sb->len = 0;
  sb->cap = 0;
  sb->data = NULL;
  return true;
}

// Appends C string and returns true.
// Returns false on failure
// On failure, logical string content and len are unchanged; builder remains
// valid; capacity may have changed. Note: always use valid (initialized) sb
// object.
bool sb_push_cstr(StrBuilder *sb, const char *cstr) {
  if (sb == NULL || cstr == NULL)
    return false;
  size_t n = strlen(cstr);
  return sb_push_n(sb, cstr, n);
}

// Appends one char, returns true on success.
// On failure, logical string content and len are unchanged; builder remains
// valid; capacity may have changed. Note: always use valid (initialized)
// string builder object.
bool sb_push_char(StrBuilder *sb, const char c) {
  if (sb == NULL || c == '\0')
    return false;
  return sb_push_n(sb, &c, 1);
}
