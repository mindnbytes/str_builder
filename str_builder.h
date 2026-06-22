#ifndef STR_BUILDER_H
#define STR_BUILDER_H

#include <stdbool.h>
#include <stddef.h>

// String Builder Invariants
// Valid iff initialized: sb_init(&sb)
// Valid object has:
// - data != NULL
// - cap >= 1 (allocated bytes)
// - len + 1 <= cap
// - len counts meaningful characters only
// - data[len] = '\0'
// - data is always a valid C string
// Invalid after sb_free(&sb)
typedef struct {
  size_t len;
  size_t cap;
  char *data;
} StrBuilder;

bool sb_init(StrBuilder *sb);
bool sb_free(StrBuilder *sb);
bool sb_push_cstr(StrBuilder *sb, const char *cstr);
bool sb_push_char(StrBuilder *sb, const char c);

#endif
