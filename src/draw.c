#include "draw.h"
#include "nodes.h"
#include <unistd.h>

static void draw_clear() {
    write(STDOUT_FILENO, "\x1b[?25l\x1b[H", 10);
}

static void draw_text(struct node* this, uint32_t size_y, enum bool is_cursor) {
    struct node* itr = this;
    uint32_t i;
    for (i = 0; itr->prev != NULL && i < size_y / 3 + 1;) {
        itr = itr->prev;
        if (itr->ch == '\n') {
            i++;
        }
    }
    if (itr->ch == '\n' && itr->prev != NULL) {
        itr = itr->next;
    }
    for (i = 0; i < size_y && itr != NULL;) {
        if (itr == this && is_cursor == true) {
            write(STDOUT_FILENO, "\x1b[7m|\x1b[0m", 9);
        }
        if (itr->ch == '\n') {
            write(STDOUT_FILENO, "\x1b[K", 3);
            i++;
        }
        write(STDOUT_FILENO, &itr->ch, 1);
        itr = itr->next;
    }
    write(STDOUT_FILENO, "\x1b[K", 3);
    for (; i < size_y; i++) {
        write(STDOUT_FILENO, "\n\x1b[K", 4);
    }
}

static void draw_info(enum mode mode) {
    if (mode == mode_insert) {
        write(STDOUT_FILENO, "\x1b[48;5;46m\x1b[30m[INSERT_MODE]\x1b[0m", 33);
    } else if (mode == mode_normal) {
        write(STDOUT_FILENO, "\x1b[48;5;39m\x1b[30m[NORMAL_MODE]\x1b[0m", 33);
    } else if (mode == mode_raw) {
        write(STDOUT_FILENO, "\x1b[48;5;226m\x1b[30m[RAW_MODE]\x1b[0m", 30);
    } else if (mode == mode_cmd) {
        write(STDOUT_FILENO, "\x1b[48;5;196m\x1b[37m[CMD_MODE]\x1b[0m", 30);
    }
}

static void draw_message(struct node* message_selector) {
    if (message_selector->prev != NULL) {
        write(STDOUT_FILENO, ", message: [", 12);
        draw_text(message_selector, 1, false);
        write(STDOUT_FILENO, "]", 1);
    }
}

static void draw_cmd(struct node* cmd_selector) {
    if (cmd_selector->prev != NULL) {
        write(STDOUT_FILENO, ", cmd: ", 7);
        draw_text(cmd_selector, 1, false);
    }
}

void draw_update(struct global* global) {
    draw_clear();
    draw_info(global->mode);
    draw_message(global->nodes.message_selector);
    draw_cmd(global->nodes.cmd_selector);
    write(STDOUT_FILENO, "\x1b[K\n", 4);
    draw_text(global->nodes.insert_selector, global->term.ws.ws_row - 2, true);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void draw_deinit() {
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
}
