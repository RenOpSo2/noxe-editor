#include "editor.h"
#include "term.h"
#include "nodes.h"
#include "input.h"
#include "draw.h"
#include <unistd.h>

void editor_init(struct global* global) {
    term_init();
    nodes_init(&global->nodes);
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
