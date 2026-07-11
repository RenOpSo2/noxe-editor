#include "draw.h"
#include "nodes.h"
#include "render_buffer.h"
#include <unistd.h>
#include <string.h>

static RenderBuffer rb;

#define RB_ESC(s) rb_append(&rb, s, sizeof(s) - 1)

// Safe compile-time length for string literals — never miscounts again.
#define ESC(s) (s), (sizeof(s) - 1)

void draw_init(void)
{
    rb_init(&rb);

    RB_ESC("\x1b[?1049h"); 
    RB_ESC("\x1b[2J");     
    RB_ESC("\x1b[H");    
    RB_ESC("\x1b[?25l");    
    rb_flush(&rb);
}

static void draw_cursor_home() {
	RB_ESC("\x1b[0m\x1b[H\x1b[2J");
}

static void draw_text(struct paged_gap_buffer* pgb, uint32_t size_y) {
    int lines = 0;
    struct page* p = pgb->active_page;
    int phase = 0;
    int idx = (int)p->gap_start - 1;

    struct page* start_p = p;
    int start_phase = 0;
    int start_idx = idx + 1;

    // Walk backward from cursor to find the scroll viewport start.
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
    // Ensure text area uses the terminal's own default colors (transparent).
    RB_ESC("\x1b[0m");

    uint32_t current_line = 0;
    p = start_p; phase = start_phase; idx = start_idx;

    while (p) {
        if (phase == 0) {
            while (idx < (int)p->gap_start) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') {
                    // Erase rest of line then newline.
                    RB_ESC("\x1b[K");
                    current_line++;
                }
		rb_append_char(&rb, ch);

                idx++;
            }
            phase = 1;
            idx = (int)p->gap_end;
            // Draw block cursor at gap position (reverse video, single space).
            if (p == pgb->active_page && current_line < size_y) {
                RB_ESC("\x1b[7m \x1b[0m");
            }
        } else {
            while (idx < PAGE_CAPACITY) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') {
                    RB_ESC("\x1b[K");
                    current_line++;
                }
		rb_append_char(&rb, ch);
                idx++;
            }
            p = p->next;
            if (p) { phase = 0; idx = 0; }
        }
    }

finish:
    // Erase current line then fill remaining rows with blank erased lines.
    RB_ESC("\x1b[K");
    for (; current_line < size_y; current_line++) {
	    RB_ESC("\n\x1b[K");
    }
}

static void draw_status(struct global* global)
{
    /* Status bar: dark bg, light fg. */
    RB_ESC("\x1b[48;5;235m\x1b[38;5;250m");

    RB_ESC(" noxe | ");

    if (global->filepath[0] != '\0') {
        rb_append(&rb,
                  global->filepath,
                  strlen(global->filepath));
    } else {
        RB_ESC("[No file]");
    }

    /* Dim hints. */
    RB_ESC("  \x1b[38;5;244mCtrl+S: Save  Ctrl+Q: Quit");

    /* Flash message in yellow (if any). */
    uint32_t msg_len = 0;
    struct page *mp = global->msg.head;

    while (mp) {
        msg_len += mp->gap_start +
                   (PAGE_CAPACITY - mp->gap_end);
        mp = mp->next;
    }

    if (msg_len > 0) {
        RB_ESC("  \x1b[38;5;220m");

        struct page *p = global->msg.head;

        while (p) {

            if (p->gap_start > 0) {
                rb_append(&rb,
                          p->data,
                          p->gap_start);
            }

            uint32_t r = PAGE_CAPACITY - p->gap_end;

            if (r > 0) {
                rb_append(&rb,
                          p->data + p->gap_end,
                          r);
            }

            p = p->next;
        }
    }

    /* Reset + erase rest of status line + newline. */
    RB_ESC("\x1b[0m\x1b[K\n");
}

void draw_update(struct global* global)
{
    rb_clear(&rb);

    draw_cursor_home();
    draw_status(global);
    draw_text(&global->text, global->term.ws.ws_row - 2);

    RB_ESC("\x1b[?25h");

    rb_flush(&rb);
}

void draw_deinit(void)
{
    rb_clear(&rb);

    RB_ESC("\x1b[?25h");
    RB_ESC("\x1b[?1049l");

    rb_flush(&rb);
}
