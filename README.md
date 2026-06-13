# str_builder

A small educational string builder written in C.

The goal of this project is to practice manual memory management, object lifetime, and invariant-based design in C.

## Design

A valid `StrBuilder` is always a valid C string.

After successful initialization, the following invariant holds:

```c
sb.data != NULL
sb.cap >= 1
sb.len + 1 <= sb.cap
sb.data[sb.len] == '\0'
```

`len` counts only meaningful characters and does not include the terminating null byte.

`cap` is the total allocated capacity, including space for the terminating `'\0'`.

## Lifecycle

A `StrBuilder` must be initialized before use:

```c
StrBuilder sb;

if (!sb_init(&sb)) {
    // handle allocation failure
}

// use sb

sb_free(&sb);
```

After `sb_free`, the builder is no longer valid for normal use unless initialized again.

## Current functionality

Currently implemented:

* `sb_init` — initialize an empty string builder
* `sb_free` — release the internal buffer and reset the object

Planned functionality:

* append a character
* append a C string
* reset while keeping allocated memory
* grow capacity as needed

## Scope

This is not intended to be a production-ready string library.

It is a learning project focused on:

* explicit ownership
* clean object lifecycle
* strong invariants
* simple error handling
* safe dynamic memory growth

