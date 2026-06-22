# str_builder

A small educational string builder written in C.

The goal of this project is to practice manual memory management, object lifetime, dynamic growth, and invariant-based design in C.

## Design

A valid `StrBuilder` is always a valid C string.

After successful initialization, the following invariant holds:

```c
sb.data != NULL
sb.cap >= 1
sb.len + 1 <= sb.cap
sb.data[sb.len] == '\0'
```

`len` counts meaningful characters only. It does not include the terminating null byte.

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

Implemented:

* `sb_init` — initialize an empty string builder
* `sb_free` — release the internal buffer and reset the object
* `sb_push_cstr` — append a null-terminated C string
* `sb_push_char` — append a char

Internal implementation details:

* the builder grows dynamically when needed
* append operations preserve a valid C-string invariant
* failed append operations leave the logical string content and `len` unchanged
* capacity may change during a failed append if growth partially succeeded

## Scope

This is not intended to be a production-ready string library.

It is a learning project focused on:

* explicit ownership
* clean object lifecycle
* strong invariants
* simple error handling
* safe dynamic memory growth
* reasoning about C strings and null termination
