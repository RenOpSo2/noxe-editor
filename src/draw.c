#include "draw.h"
#include "nodes.h"
#include <unistd.h>
#include <string.h>

static void draw_clear() {
    write(STDOUT_FILENO, "\x1b[?25l\x1b[H", 10);
}

static void draw_text(struct paged_gap_buffer* pgb, uint32_t size_y) {
    int lines = 0;
    struct page* p = pgb->active_page;
    int phase = 0;
    int idx = (int)p->gap_start - 1;

    struct page* start_p = p;
    int start_phase = 0;
    int start_idx = idx + 1;

    while (p) {
        if (phase == 0) {
            while (idx >= 0) {
                if (p->data[idx] == '\n') {
                    lines++;
                    if (lines > (int)size_y / 3) {
                        start_p = p; start_phase = 0; start_idx = idx + 1;
                        goto draw;
                    }
                }
                idx--;
            }
            p = p->prev;
            if (p) { phase = 1; idx = PAGE_CAPACITY - 1; }
        } else {
            while (idx >= (int)p->gap_end) {
                if (p->data[idx] == '\n') {
                    lines++;
                    if (lines > (int)size_y / 3) {
                        start_p = p; start_phase = 1; start_idx = idx + 1;
                        goto draw;
                    }
                }
                idx--;
            }
            phase = 0;
            idx = (int)p->gap_start - 1;
        }
    }
    start_p = pgb->head; start_phase = 0; start_idx = 0;

draw:;
    uint32_t current_line = 0;
    p = start_p; phase = start_phase; idx = start_idx;

    while (p) {
        if (phase == 0) {
            while (idx < (int)p->gap_start) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') { write(STDOUT_FILENO, "\x1b[K", 3); current_line++; }
                write(STDOUT_FILENO, &ch, 1);
                idx++;
            }
            phase = 1;
            idx = (int)p->gap_end;
            if (p == pgb->active_page && current_line < size_y) {
                // Draw a subtle block cursor
                write(STDOUT_FILENO, "\x1b[7m \x1b[0m", 8);
            }
        } else {
            while (idx < PAGE_CAPACITY) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') { write(STDOUT_FILENO, "\x1b[K", 3); current_line++; }
                write(STDOUT_FILENO, &ch, 1);
                idx++;
            }
            p = p->next;
            if (p) { phase = 0; idx = 0; }
        }
    }

finish:
    write(STDOUT_FILENO, "\x1b[K", 3);
    for (; current_line < size_y; current_line++) {
        write(STDOUT_FILENO, "\n\x1b[K", 4);
    }
}

static void draw_status(struct global* global) {
    // Full-width dark status bar
    write(STDOUT_FILENO, "\x1b[48;5;235m\x1b[38;5;250m", 21);

    // Left: filename
    write(STDOUT_FILENO, " noxe | ", 8);
    if (global->filepath[0] != '\0') {
        write(STDOUT_FILENO, global->filepath, strlen(global->filepath));
    } else {
        write(STDOUT_FILENO, "[No file]", 9);
    }

    // Hint
    write(STDOUT_FILENO, "  \x1b[38;5;244mCtrl+S: Save  Ctrl+Q: Quit", 37);

    // Message (if any)
    uint32_t msg_len = 0;
    struct page* mp = global->msg.head;
    while (mp) {
        msg_len += mp->gap_start + (PAGE_CAPACITY - mp->gap_end);
        mp = mp->next;
    }
    if (msg_len > 0) {
        write(STDOUT_FILENO, "  \x1b[38;5;220m", 12);
        struct page* p = global->msg.head;
        while (p) {
            if (p->gap_start > 0) write(STDOUT_FILENO, p->data, p->gap_start);
            uint32_t r = PAGE_CAPACITY - p->gap_end;
            if (r > 0) write(STDOUT_FILENO, p->data + p->gap_end, r);
            p = p->next;
        }
    }

    write(STDOUT_FILENO, "\x1b[0m\x1b[K\n", 8);
}

void draw_update(struct global* global) {
    draw_clear();
    draw_status(global);
    draw_text(&global->text, global->term.ws.ws_row - 2);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void draw_deinit() {
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
}
