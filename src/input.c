#include "input.h"
#include "nodes.h"
#include "file.h"
#include "term.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Escape sequence parser
// Handles plain arrows (\x1b[A-D) and Shift+arrows (\x1b[1;2A-D).
// ---------------------------------------------------------------------------
typedef enum {
    esc_none,
    esc_got_esc,     // received \x1b
    esc_got_bracket, // received \x1b[
    esc_got_1,       // received \x1b[1
    esc_got_semi,    // received \x1b[1;
    esc_got_2,       // received \x1b[1;2
} esc_state;

static esc_state esc = esc_none;

// ---------------------------------------------------------------------------
// Selection helpers
// ---------------------------------------------------------------------------
static void sel_begin(struct global* g) {
    if (!g->has_selection) {
        g->sel_anchor = pgb_cursor_pos(&g->text);
        g->has_selection = true;
    }
}

static void sel_clear(struct global* g) {
    g->has_selection = false;
}

// Return normalised [start, end) for the current selection.
static void sel_range(struct global* g, uint32_t* from, uint32_t* to) {
    uint32_t cursor = pgb_cursor_pos(&g->text);
    if (g->sel_anchor <= cursor) {
        *from = g->sel_anchor;
        *to   = cursor;
    } else {
        *from = cursor;
        *to   = g->sel_anchor;
    }
}

// Delete the selected region, position cursor at from, clear selection.
static void sel_delete(struct global* g) {
    uint32_t from, to;
    sel_range(g, &from, &to);
    pgb_delete_range(&g->text, from, to);
    sel_clear(g);
}

// ---------------------------------------------------------------------------
// Input handler
// ---------------------------------------------------------------------------
enum result input_update(struct global* global) {
    char buf[term_capacity];
    uint32_t n = term_read(buf);

    for (uint32_t i = 0; i < n; i++) {
        unsigned char ch = (unsigned char)buf[i];

        // ---- Escape sequence state machine --------------------------------
        if (esc == esc_none && ch == 0x1b) {
            esc = esc_got_esc;
            continue;
        }
        if (esc == esc_got_esc) {
            if (ch == '[') { esc = esc_got_bracket; continue; }
            esc = esc_none; continue; // unknown, discard
        }
        if (esc == esc_got_bracket) {
            // Plain arrow
            if (ch == 'A') { sel_clear(global); pgb_move_up(&global->text);    esc = esc_none; continue; }
            if (ch == 'B') { sel_clear(global); pgb_move_down(&global->text);  esc = esc_none; continue; }
            if (ch == 'C') { sel_clear(global); pgb_move_right(&global->text); esc = esc_none; continue; }
            if (ch == 'D') { sel_clear(global); pgb_move_left(&global->text);  esc = esc_none; continue; }
            // Start of modifier sequence \x1b[1
            if (ch == '1') { esc = esc_got_1; continue; }
            esc = esc_none; continue;
        }
        if (esc == esc_got_1) {
            if (ch == ';') { esc = esc_got_semi; continue; }
            esc = esc_none; continue;
        }
        if (esc == esc_got_semi) {
            if (ch == '2') { esc = esc_got_2; continue; } // modifier = Shift
            esc = esc_none; continue;
        }
        if (esc == esc_got_2) {
            // Shift + Arrow → extend selection
            esc = esc_none;
            sel_begin(global); // set anchor if first shift-arrow
            switch (ch) {
                case 'A': pgb_move_up(&global->text);    continue;
                case 'B': pgb_move_down(&global->text);  continue;
                case 'C': pgb_move_right(&global->text); continue;
                case 'D': pgb_move_left(&global->text);  continue;
                default:  continue;
            }
        }

        // ---- Ctrl+Q: quit -------------------------------------------------
        if (ch == CTRL_KEY('q')) {
            return err;
        }

        // ---- Ctrl+S: save -------------------------------------------------
        if (ch == CTRL_KEY('s')) {
            sel_clear(global);
            if (global->filepath[0] != '\0') {
                if (file_write(global->filepath, &global->text) == ok) {
                    pgb_replace_str(&global->msg, "Saved.", &global->arena);
                } else {
                    pgb_replace_str(&global->msg, "Save failed!", &global->arena);
                }
            } else {
                pgb_replace_str(&global->msg, "No filepath — pass one as argument.", &global->arena);
            }
            continue;
        }

        // ---- Ctrl+C: copy -------------------------------------------------
        if (ch == CTRL_KEY('c')) {
            if (global->has_selection) {
                uint32_t from, to;
                sel_range(global, &from, &to);
                pgb_copy_range(&global->clipboard, &global->text, from, to, &global->arena);
                pgb_replace_str(&global->msg, "Copied.", &global->arena);
                sel_clear(global);
            }
            continue;
        }

        // ---- Ctrl+X: cut --------------------------------------------------
        if (ch == CTRL_KEY('x')) {
            if (global->has_selection) {
                uint32_t from, to;
                sel_range(global, &from, &to);
                pgb_copy_range(&global->clipboard, &global->text, from, to, &global->arena);
                pgb_delete_range(&global->text, from, to);
                pgb_replace_str(&global->msg, "Cut.", &global->arena);
                sel_clear(global);
            }
            continue;
        }

        // ---- Ctrl+V: paste ------------------------------------------------
        if (ch == CTRL_KEY('v')) {
            // If something is selected, replace it with clipboard.
            if (global->has_selection) {
                sel_delete(global);
            }
            struct page* p = global->clipboard.head;
            while (p) {
                for (uint32_t j = 0; j < p->gap_start; j++)
                    pgb_insert(&global->text, p->data[j], &global->arena);
                for (uint32_t j = p->gap_end; j < PAGE_CAPACITY; j++)
                    pgb_insert(&global->text, p->data[j], &global->arena);
                p = p->next;
            }
            pgb_replace_str(&global->msg, "Pasted.", &global->arena);
            continue;
        }

        // ---- Ctrl+A: select all -------------------------------------------
        if (ch == CTRL_KEY('a')) {
            pgb_move_to_pos(&global->text, 0);
            global->sel_anchor = 0;
            global->has_selection = true;
            // Move cursor to end
            while (1) {
                struct page* p = global->text.active_page;
                if (p->gap_end == PAGE_CAPACITY && !p->next) break;
                pgb_move_right(&global->text);
            }
            pgb_replace_str(&global->msg, "Selected all.", &global->arena);
            continue;
        }

        // ---- Regular text input -------------------------------------------
        sel_clear(global); // any non-special key clears selection unless it's a replace

        if (ch == '\b' || ch == 127) {
            pgb_delete(&global->text);
        } else if (ch == '\r') {
            pgb_insert(&global->text, '\n', &global->arena);
        } else if (ch >= 32 || ch == '\t' || ch == '\n') {
            pgb_insert(&global->text, (char)ch, &global->arena);
        }
    }
    return ok;
}
