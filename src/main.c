#include "editor.h"
#include <unistd.h>

int main(void) {
    static struct global global;
    editor_init(&global);
    while (1) {
        if (editor_update(&global) == ok) {
            usleep(10000);
        } else {
            break;
        }
    }
    editor_deinit(&global);
    return 0;
}
