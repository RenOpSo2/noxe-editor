#include "input.h"
#include "nodes.h"
#include "file.h"
#include "term.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Arrow key escape sequences are: \x1b [ A/B/C/D
// We track a small escape-sequence state machine.
typedef enum {
    esc_none,
    esc_bracket,  // received \x1b
    esc_arrow,    // received \x1b[
} esc_state;

static esc_state esc = esc_none;

enum result input_update(struct global* global) {
    char buf[term_capacity];
    uint32_t n = term_read(buf);

    for (uint32_t i = 0; i < n; i++) {
        unsigned char ch = (unsigned char)buf[i];

        // --- Escape sequence state machine ---
        if (esc == esc_none && ch == 0x1b) {
            esc = esc_bracket;
            continue;
        }
        if (esc == esc_bracket) {
            if (ch == '[') { esc = esc_arrow; continue; }
            esc = esc_none;  // unrecognised, discard
            continue;
        }
        if (esc == esc_arrow) {
            esc = esc_none;
            switch (ch) {
                case 'A': pgb_move_up(&global->text);    continue;
                case 'B': pgb_move_down(&global->text);  continue;
                case 'C': pgb_move_right(&global->text); continue;
                case 'D': pgb_move_left(&global->text);  continue;
                default:  continue;
            }
        }

        // --- Control keys ---
        if (ch == CTRL_KEY('q')) {
            return err;  // quit
        }
        if (ch == CTRL_KEY('s')) {
            if (global->filepath[0] != '\0') {
                if (file_write(global->filepath, &global->text) == ok) {
                    pgb_replace_str(&global->msg, "Saved.", &global->arena);
                } else {
                    pgb_replace_str(&global->msg, "Save failed!", &global->arena);
                }
            } else {
                pgb_replace_str(&global->msg, "No file — open with Ctrl+O.", &global->arena);
            }
            continue;
        }
        if (ch == CTRL_KEY('o')) {
            // Ctrl+O: placeholder for future open-file prompt
            pgb_replace_str(&global->msg, "Open: pass filepath as argument.", &global->arena);
            continue;
        }

        // --- Regular text input ---
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
