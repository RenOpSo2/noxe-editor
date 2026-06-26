#include "input.h"
#include "cmd.h"
#include "nodes.h"
#include "term.h"
#include <stddef.h>
#include <stdint.h>

static void input_normal_h(struct nodes* nodes) {
    if (nodes->insert_selector->prev != NULL) {
        nodes->insert_selector = nodes->insert_selector->prev;
    }
}

static void input_normal_l(struct nodes* nodes) {
    if (nodes->insert_selector->next != NULL) {
        nodes->insert_selector = nodes->insert_selector->next;
    }
}

static void input_normal_j(struct nodes* nodes) {
    uint32_t x = nodes_line_left(nodes->insert_selector);
    nodes->insert_selector = nodes_line_rbegin(nodes->insert_selector);
    input_normal_l(nodes);
    for (uint32_t i = 0; i < x && nodes->insert_selector != NULL && nodes->insert_selector->ch != '\n'; i++) {
        input_normal_l(nodes);
    }
}

static void input_normal_k(struct nodes* nodes) {
    uint32_t x = nodes_line_left(nodes->insert_selector);
    nodes->insert_selector = nodes_line_begin(nodes->insert_selector);
    input_normal_h(nodes);
    nodes->insert_selector = nodes_line_begin(nodes->insert_selector);
    for (uint32_t i = 0; i < x && nodes->insert_selector != NULL && nodes->insert_selector->ch != '\n'; i++) {
        input_normal_l(nodes);
    }
}

static void input_normal(struct global* global, char ch) {
    switch (ch) {
        case 'i':
            global->mode = mode_insert;
            return;
        case ':':
            global->mode = mode_cmd;
            return;
        case 'h':
            input_normal_h(&global->nodes);
            return;
        case 'l':
            input_normal_l(&global->nodes);
            return;
        case 'j':
            input_normal_j(&global->nodes);
            return;
        case 'k':
            input_normal_k(&global->nodes);
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
            if (cmd_exec(global, global->nodes.cmd_selector) == ok) {
                nodes_clear(&global->nodes, global->nodes.cmd_selector);
                global->mode = mode_normal;
                return ok;
            } else {
                return err;
            }
        case '\b':
        case 127:
            if (global->nodes.cmd_selector->prev != NULL) {
                nodes_delete(&global->nodes, global->nodes.cmd_selector->prev);
            }
            return ok;
        default:
            nodes_insert(&global->nodes, global->nodes.cmd_selector, ch);
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
            if (global->nodes.insert_selector->prev != NULL) {
                nodes_delete(&global->nodes, global->nodes.insert_selector->prev);
            }
            return;
        default:
            nodes_insert(&global->nodes, global->nodes.insert_selector, ch);
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
