#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include "../libmemory/arena.h"

#define arena_capacity (1 << 24) // 16 MB arena
#define term_capacity (1 << 16)
#define buf_capacity (1 << 16)

enum result {
    ok = 0,
    err = 1,
};
enum bool {
    false = 0,
    true = 1,
};
enum mode {
    mode_normal,
    mode_insert,
    mode_cmd,
    mode_raw,
};

struct gap_buffer {
    char* data;
    uint32_t capacity;
    uint32_t gap_start;
    uint32_t gap_end;
};

struct term {
    struct winsize ws;
};

struct global {
    struct term term;
    struct gap_buffer text;
    struct gap_buffer cmd;
    struct gap_buffer msg;
    enum mode mode;
    Arena arena;
};

#endif
