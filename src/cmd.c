#include "cmd.h"
#include "file.h"
#include "nodes.h"
#include <string.h>

enum result cmd_openfile(struct gap_buffer* gb, const char* path, Arena* arena) {
    gb_clear(gb);
    return file_read(gb, path, arena);
}

enum result cmd_savefile(struct gap_buffer* gb, const char* path) {
    return file_write(path, gb);
}

enum result cmd_exec(struct global* global, struct gap_buffer* cmd_buf) {
    char buf[buf_capacity];
    char* option;
    gb_to_str(buf, cmd_buf);
    gb_clear(&global->msg);
    
    uint32_t i = 0;
    while (buf[i] != ' ' && buf[i] != '\0') {
        i++;
    }
    if (buf[i] == ' ') {
        buf[i++] = '\0';
    }
    option = buf + i;
    
    if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0 || strcmp(buf, "q") == 0) {
        return err;
    } else if (strcmp(buf, "open") == 0) {
        if (cmd_openfile(&global->text, option, &global->arena) == ok) {
            gb_replace_str(&global->msg, "open succeeded.", &global->arena);
        } else {
            gb_replace_str(&global->msg, "open failed.", &global->arena);
        }
        return ok;
    } else if (strcmp(buf, "save") == 0) {
        if (cmd_savefile(&global->text, option) == ok) {
            gb_replace_str(&global->msg, "save succeeded.", &global->arena);
        } else {
            gb_replace_str(&global->msg, "save failed.", &global->arena);
        }
        return ok;
    } else {
        gb_replace_str(&global->msg, "command not found.", &global->arena);
        return ok;
    }
}
