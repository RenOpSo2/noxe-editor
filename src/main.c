#include "editor.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    printf("DEBUG: main start\n"); fflush(stdout);
    static struct global global;
    const char* filepath = (argc > 1) ? argv[1] : NULL;
    printf("DEBUG: calling editor_init\n"); fflush(stdout);
    editor_init(&global, filepath);
    printf("DEBUG: editor_init done\n"); fflush(stdout);
    while (1) {
        printf("DEBUG: editor_update loop\n"); fflush(stdout);
        if (editor_update(&global) != ok) {
            break;
        }
    }
    printf("DEBUG: calling editor_deinit\n"); fflush(stdout);
    editor_deinit(&global);
    printf("DEBUG: main end\n"); fflush(stdout);
    return 0;
}
