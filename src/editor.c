#include "editor.h"
#include "term.h"
#include "nodes.h"
#include "input.h"
#include "draw.h"
#include <unistd.h>

void editor_init(struct global* global) {
    term_init();
    
    static char arena_mem[arena_capacity];
    global->arena = arena_init(arena_mem, sizeof(arena_mem));
    
    gb_init(&global->text, 1024 * 1024, &global->arena);
    gb_init(&global->cmd, 256, &global->arena);
    gb_init(&global->msg, 256, &global->arena);
    
    global->mode = mode_normal;
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
