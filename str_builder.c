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
// Trust caller that pointed to memory
// has at least n characters to read.
// Performs atomic mutation. If it returns false,
// it guarantees the provided pointer hasn't been changed.
// Writes n characters from src to internal buffer and
// returns true. Preserves valid string builder invariants.
static bool sb_push_n(StrBuilder *sb, const char *src, size_t n) {
  // no-op
  if (n == 0)
    return true;
  // grow
  if (sb->len + n + 1 > sb->cap) {
    if (!sb_grow(sb))
      return false;
  }
  // copy
  char *dst = sb->data + sb->len;
  strncpy(dst, src, n);
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
// Returns false on failure, doesn't mutate
// StrBuilder object in this case.
// Note: always use valid (initialized) sb object.
bool sb_push_cstr(StrBuilder *sb, const char *cstr) {
  if (sb == NULL || cstr == NULL)
    return false;
  size_t n = strlen(cstr);
  return sb_push_n(sb, cstr, n);
}
