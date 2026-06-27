#include "editor.h"
#include "term.h"
#include "nodes.h"
#include "input.h"
#include "draw.h"
#include "file.h"
#include <string.h>
#include <unistd.h>

void editor_init(struct global* global, const char* filepath) {
    term_init();
    draw_init();

    static char arena_mem[arena_capacity];
    global->arena = arena_init(arena_mem, sizeof(arena_mem));

    pgb_init(&global->text, &global->arena);
    pgb_init(&global->msg, &global->arena);

    global->filepath[0] = '\0';

    if (filepath) {
        strncpy(global->filepath, filepath, sizeof(global->filepath) - 1);
        global->filepath[sizeof(global->filepath) - 1] = '\0';
        if (file_read(&global->text, global->filepath, &global->arena) != ok) {
            pgb_replace_str(&global->msg, "Could not open file.", &global->arena);
        }
    }
}

void editor_deinit(struct global* global) {
    (void)global;
    draw_deinit();
    term_deinit();
}

enum result editor_update(struct global* global) {
    if (input_update(global) == err) {
        return err;
    }
    term_update(&global->term);
    draw_update(global);
    return ok;
}
