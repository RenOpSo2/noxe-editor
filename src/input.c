#include "input.h"
#include "cmd.h"
#include "nodes.h"
#include "term.h"
#include <stddef.h>
#include <stdint.h>

static void input_normal(struct global* global, char ch) {
    switch (ch) {
        case 'i':
            global->mode = mode_insert;
            return;
        case ':':
            global->mode = mode_cmd;
            return;
        case 'h':
            pgb_move_left(&global->text);
            return;
        case 'l':
            pgb_move_right(&global->text);
            return;
        case 'j':
            pgb_move_down(&global->text);
            return;
        case 'k':
            pgb_move_up(&global->text);
            return;
        default:
            return;
    }
}

static enum result input_cmd(struct global* global, char ch) {
    switch (ch) {
        case 27:
            global->mode = mode_normal;
            return ok;
        case '\n':
            if (cmd_exec(global, &global->cmd) == ok) {
                pgb_clear(&global->cmd);
                global->mode = mode_normal;
                return ok;
            } else {
                return err;
            }
        case '\b':
        case 127:
            pgb_delete(&global->cmd);
            return ok;
        default:
            pgb_insert(&global->cmd, ch, &global->arena);
            return ok;
    }
}

static void input_insert(struct global* global, char ch) {
    switch (ch) {
        case 27:
            global->mode = mode_normal;
            return;
        case '\b':
        case 127:
            pgb_delete(&global->text);
            return;
        default:
            pgb_insert(&global->text, ch, &global->arena);
            return;
    }
}

static enum result input_ch(struct global* global, char ch) {
    if (global->mode == mode_normal) {
        input_normal(global, ch);
        return ok;
    }
    if (global->mode == mode_insert) {
        input_insert(global, ch);
        return ok;
    }
    if (global->mode == mode_cmd) {
        return input_cmd(global, ch);
    } else {
        return err;
    }
}

enum result input_update(struct global* global) {
    char buf[term_capacity];
    uint32_t n = term_read(buf);
    for (uint32_t i = 0; i < n; i++) {
        if (input_ch(global, buf[i]) == ok) {
            continue;
        } else {
            return err;
        }
    }
    return ok;
}
