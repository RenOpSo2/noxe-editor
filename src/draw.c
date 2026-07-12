#include "draw.h"
#include "nodes.h"
#include "render_buffer.h"
#include "syntax.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static RenderBuffer rb;

#define RB_ESC(s) rb_append(&rb, s, sizeof(s) - 1)

// Safe compile-time length for string literals — never miscounts again.
#define ESC(s) (s), (sizeof(s) - 1)

void draw_init(void)
{
    printf("DEBUG: draw_init start\n"); fflush(stdout);
    rb_init(&rb);

    RB_ESC("\x1b[?1049h"); 
    RB_ESC("\x1b[2J");     
    RB_ESC("\x1b[H");    
    RB_ESC("\x1b[?25l");    
    rb_flush(&rb);

    // Initialize syntax highlighting
    // syntax_init(); // Disable for debugging
    printf("DEBUG: draw_init end\n"); fflush(stdout);
}

static void draw_cursor_home() {
	RB_ESC("\x1b[0m\x1b[H\x1b[2J");
}

static void draw_text(struct paged_gap_buffer* pgb, uint32_t size_y) {
    printf("DEBUG: draw_text start, size_y=%u\n", size_y); fflush(stdout);
    int lines = 0;
    struct page* p = pgb->active_page;
    printf("DEBUG: pgb->active_page=%p\n", (void*)p); fflush(stdout);
    if (!p) {
        printf("DEBUG: active_page is NULL!\n"); fflush(stdout);
        return;
    }
    int phase = 0;
    int idx = (int)p->gap_start - 1;
    printf("DEBUG: idx=%d, gap_start=%u\n", idx, p->gap_start); fflush(stdout);

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
    
    // Buffer to collect line content for syntax highlighting
    char line_buffer[8192];
    int line_pos = 0;

    while (p) {
        if (phase == 0) {
            while (idx < (int)p->gap_start) {
                if (current_line >= size_y) goto finish;
                char ch = p->data[idx];
                if (ch == '\n') {
                    // Null-terminate the line
                    line_buffer[line_pos] = '\0';
                    
                    // Apply syntax highlighting if available
                    // if (syntax_is_available() && line_pos > 0) {
                    //     char* highlighted = syntax_highlight_line(line_buffer, line_pos);
                    //     if (highlighted) {
                    //         rb_append(&rb, highlighted, strlen(highlighted));
                    //         free(highlighted);
                    //     } else {
                    //         // Fallback to original text
                    //         rb_append(&rb, line_buffer, line_pos);
                    //     }
                    // } else {
                        // No syntax highlighting, output as-is
                        rb_append(&rb, line_buffer, line_pos);
                    // }
                    
                    // Erase rest of line then newline.
                    RB_ESC("\x1b[K");
                    current_line++;
                    
                    // Reset line buffer
                    line_pos = 0;
                } else {
                    // Add character to line buffer
                    if (line_pos < (int)sizeof(line_buffer) - 1) {
                        line_buffer[line_pos++] = ch;
                    }
                }
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
                    // Null-terminate the line
                    line_buffer[line_pos] = '\0';
                    
                    // Apply syntax highlighting if available
                    // if (syntax_is_available() && line_pos > 0) {
                    //     char* highlighted = syntax_highlight_line(line_buffer, line_pos);
                    //     if (highlighted) {
                    //         rb_append(&rb, highlighted, strlen(highlighted));
                    //         free(highlighted);
                    //     } else {
                    //         // Fallback to original text
                    //         rb_append(&rb, line_buffer, line_pos);
                    //     }
                    // } else {
                        // No syntax highlighting, output as-is
                        rb_append(&rb, line_buffer, line_pos);
                    // }
                    
                    RB_ESC("\x1b[K");
                    current_line++;
                    
                    // Reset line buffer
                    line_pos = 0;
                } else {
                    // Add character to line buffer
                    if (line_pos < (int)sizeof(line_buffer) - 1) {
                        line_buffer[line_pos++] = ch;
                    }
                }
                idx++;
            }
            p = p->next;
            if (p) { phase = 0; idx = 0; }
        }
    }
    
    // Handle any remaining content in the buffer
    if (line_pos > 0 && current_line < size_y) {
        line_buffer[line_pos] = '\0';
        if (syntax_is_available()) {
            char* highlighted = syntax_highlight_line(line_buffer, line_pos);
            if (highlighted) {
                rb_append(&rb, highlighted, strlen(highlighted));
                free(highlighted);
            } else {
                rb_append(&rb, line_buffer, line_pos);
            }
        } else {
            rb_append(&rb, line_buffer, line_pos);
        }
        RB_ESC("\x1b[K");
        current_line++;
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
    printf("DEBUG: draw_status start\n"); fflush(stdout);
    /* Status bar: dark bg, light fg. */
    RB_ESC("\x1b[48;5;235m\x1b[38;5;250m");

    RB_ESC(" noxe | ");

    printf("DEBUG: before filepath check\n"); fflush(stdout);
    if (global->filepath[0] != '\0') {
        printf("DEBUG: filepath is not empty\n"); fflush(stdout);
        rb_append(&rb,
                  global->filepath,
                  strlen(global->filepath));
    } else {
        printf("DEBUG: filepath is empty\n"); fflush(stdout);
        RB_ESC("[No file]");
    }

    /* Dim hints. */
    RB_ESC("  \x1b[38;5;244mCtrl+S: Save  Ctrl+Q: Quit");

    printf("DEBUG: before message processing\n"); fflush(stdout);
    /* Flash message in yellow (if any). */
    uint32_t msg_len = 0;
    struct page *mp = global->msg.head;
    printf("DEBUG: global->msg.head=%p\n", (void*)mp); fflush(stdout);

    while (mp) {
        printf("DEBUG: processing message page\n"); fflush(stdout);
        msg_len += mp->gap_start +
                   (PAGE_CAPACITY - mp->gap_end);
        mp = mp->next;
    }
    printf("DEBUG: message processing done, msg_len=%u\n", msg_len); fflush(stdout);

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
    printf("DEBUG: draw_status end\n"); fflush(stdout);
}

void draw_update(struct global* global)
{
    printf("DEBUG: draw_update start\n"); fflush(stdout);
    rb_clear(&rb);

    draw_cursor_home();
    printf("DEBUG: after draw_cursor_home\n"); fflush(stdout);
    draw_status(global);
    printf("DEBUG: after draw_status\n"); fflush(stdout);
    draw_text(&global->text, global->term.ws.ws_row - 2);
    printf("DEBUG: after draw_text\n"); fflush(stdout);

    RB_ESC("\x1b[?25h");

    rb_flush(&rb);
    printf("DEBUG: draw_update end\n"); fflush(stdout);
}

void draw_deinit(void)
{
    rb_clear(&rb);

    RB_ESC("\x1b[?25h");
    RB_ESC("\x1b[?1049l");

    rb_flush(&rb);

    // Cleanup syntax highlighting
    // syntax_deinit(); // Disable for debugging
}
