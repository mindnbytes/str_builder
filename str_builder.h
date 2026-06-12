#ifndef STR_BUILDER_H
#define STR_BUILDER_H

#include <stdbool.h>
#include <stddef.h>

// String Builder Invariants
// Valid iff initialized: st_init(&sb)
// Valid object has:
// - len >= 0 (meaningful characters)
// - cap >= 1 (allocated bytes)
// - data is allocated
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

#endif
