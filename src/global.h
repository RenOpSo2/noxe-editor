#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <sys/ioctl.h>  // struct winsize butuh ini
#include <stddef.h>
#include "../libmemory/arena.h"

#define nodes_capacity (1 << 18)
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

struct node {
    struct node* next;
    struct node* prev;
    char ch;
};

struct nodes {
    Arena arena;
    struct node* passive_selector;
    struct node* insert_selector;
    struct node* cmd_selector;
    struct node* message_selector;
};

struct term {
    struct winsize ws;
};

struct global {
    struct term term;
    struct nodes nodes;
    enum mode mode;
};

#endif
