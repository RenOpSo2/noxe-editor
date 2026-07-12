#include "src/editor.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    printf("DEBUG: main start\n");
    static struct global global;
    const char* filepath = (argc > 1) ? argv[1] : NULL;
    printf("DEBUG: filepath=%s\n", filepath ? filepath : "NULL");
    printf("DEBUG: calling editor_init\n");
    editor_init(&global, filepath);
    printf("DEBUG: editor_init done\n");
    
    printf("DEBUG: calling editor_update once\n");
    if (editor_update(&global) != ok) {
        printf("DEBUG: editor_update returned error\n");
    }
    printf("DEBUG: first update done\n");
    
    printf("DEBUG: calling editor_deinit\n");
    editor_deinit(&global);
    printf("DEBUG: main end\n");
    return 0;
}