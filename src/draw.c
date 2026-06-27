#include "draw.h"
#include "nodes.h"
#include <unistd.h>

static void draw_clear() {
    write(STDOUT_FILENO, "\x1b[?25l\x1b[H", 10);
}

static void draw_text(struct paged_gap_buffer* pgb, uint32_t size_y) {
    int lines = 0;
    struct page* p = pgb->active_page;
    int phase = 0;
    int idx = p->gap_start - 1;
    
    struct page* start_p = p;
    int start_phase = 0;
    int start_idx = idx + 1;
    
    while (p) {
        if (phase == 0) {
            while (idx >= 0) {
                if (p->data[idx] == '\n') {
                    lines++;
                    if (lines > (int)size_y / 3) {
                        start_p = p;
                        start_phase = 0;
                        start_idx = idx + 1;
                        goto draw;
                    }
                }
                idx--;
            }
            p = p->prev;
            if (p) {
                phase = 1;
                idx = PAGE_CAPACITY - 1;
            }
        } else {
            while (idx >= (int)p->gap_end) {
                if (p->data[idx] == '\n') {
                    lines++;
                    if (lines > (int)size_y / 3) {
                        start_p = p;
                        start_phase = 1;
                        start_idx = idx + 1;
                        goto draw;
                    }
                }
                idx--;
            }
            phase = 0;
            idx = p->gap_start - 1;
        }
    }
    start_p = pgb->head;
    start_phase = 0;
    start_idx = 0;

draw:
    ;
    uint32_t current_line = 0;
    p = start_p;
    phase = start_phase;
    idx = start_idx;
    
    while (p) {
        if (phase == 0) {
            while (idx < (int)p->gap_start) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') {
                    write(STDOUT_FILENO, "\x1b[K", 3);
                    current_line++;
                }
                write(STDOUT_FILENO, &ch, 1);
                idx++;
            }
            phase = 1;
            idx = p->gap_end;
            if (p == pgb->active_page && current_line < size_y) {
                write(STDOUT_FILENO, "\x1b[7m|\x1b[0m", 9);
            }
        } else {
            while (idx < PAGE_CAPACITY) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') {
                    write(STDOUT_FILENO, "\x1b[K", 3);
                    current_line++;
                }
                write(STDOUT_FILENO, &ch, 1);
                idx++;
            }
            p = p->next;
            if (p) {
                phase = 0;
                idx = 0;
            }
        }
    }

finish:
    write(STDOUT_FILENO, "\x1b[K", 3);
    for (; current_line < size_y; current_line++) {
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

static void draw_buffer_raw(struct paged_gap_buffer* pgb) {
    struct page* p = pgb->head;
    while (p) {
        if (p->gap_start > 0) write(STDOUT_FILENO, p->data, p->gap_start);
        uint32_t right = PAGE_CAPACITY - p->gap_end;
        if (right > 0) write(STDOUT_FILENO, p->data + p->gap_end, right);
        p = p->next;
    }
}

static void draw_message(struct paged_gap_buffer* msg) {
    if (msg->head->gap_start > 0 || (PAGE_CAPACITY - msg->head->gap_end) > 0 || msg->head->next) {
        write(STDOUT_FILENO, ", message: [", 12);
        draw_buffer_raw(msg);
        write(STDOUT_FILENO, "]", 1);
    }
}

static void draw_cmd(struct paged_gap_buffer* cmd) {
    if (cmd->head->gap_start > 0 || (PAGE_CAPACITY - cmd->head->gap_end) > 0 || cmd->head->next) {
        write(STDOUT_FILENO, ", cmd: ", 7);
        draw_buffer_raw(cmd);
    }
}

void draw_update(struct global* global) {
    draw_clear();
    draw_info(global->mode);
    draw_message(&global->msg);
    draw_cmd(&global->cmd);
    write(STDOUT_FILENO, "\x1b[K\n", 4);
    draw_text(&global->text, global->term.ws.ws_row - 2);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void draw_deinit() {
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
}
