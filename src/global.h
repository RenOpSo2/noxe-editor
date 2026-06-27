#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include "../libmemory/arena.h"

#define arena_capacity (1 << 24)
#define term_capacity (1 << 16)
#define buf_capacity (1 << 16)

#define PAGE_CAPACITY 4096

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

struct page {
    char data[PAGE_CAPACITY];
    uint32_t gap_start;
    uint32_t gap_end;
    struct page* next;
    struct page* prev;
};

struct paged_gap_buffer {
    struct page* head;
    struct page* tail;
    struct page* active_page;
};

struct term {
    struct winsize ws;
};

struct global {
    struct term term;
    struct paged_gap_buffer text;
    struct paged_gap_buffer cmd;
    struct paged_gap_buffer msg;
    enum mode mode;
    Arena arena;
};

#endif
