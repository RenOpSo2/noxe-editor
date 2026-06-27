#include "editor.h"
#include <unistd.h>

int main(int argc, char* argv[]) {
    static struct global global;
    const char* filepath = (argc > 1) ? argv[1] : NULL;
    editor_init(&global, filepath);
    while (1) {
        if (editor_update(&global) != ok) {
            break;
        }
    }
    editor_deinit(&global);
    return 0;
}
