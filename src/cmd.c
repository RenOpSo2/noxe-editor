#include "cmd.h"
#include "file.h"
#include "nodes.h"
#include <string.h>

enum result cmd_openfile(struct nodes* nodes, const char* path) {
    nodes_clear(nodes, nodes->insert_selector);
    return file_read(nodes, nodes->insert_selector, path);
}

enum result cmd_savefile(struct nodes* nodes, const char* path) {
    return file_write(path, nodes->insert_selector);
}

enum result cmd_exec(struct global* global, struct node* this) {
    char buf[buf_capacity];
    char* option;
    uint32_t i = 0;
    nodes_to_str(buf, this);
    nodes_clear(&global->nodes, global->nodes.message_selector);
    while (buf[i] != ' ' && buf[i] != '\0') {
        i++;
    }
    buf[i++] = '\0';
    option = buf + i;
    if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0 || strcmp(buf, "q") == 0) {
        return err;
    } else if (strcmp(buf, "open") == 0) {
        if (cmd_openfile(&global->nodes, option) == ok) {
            nodes_replace_str(&global->nodes, global->nodes.message_selector, "open succeeded.");
        } else {
            nodes_replace_str(&global->nodes, global->nodes.message_selector, "open failed.");
        }
        return ok;
    } else if (strcmp(buf, "save") == 0) {
        if (cmd_savefile(&global->nodes, option) == ok) {
            nodes_replace_str(&global->nodes, global->nodes.message_selector, "save succeeded.");
        } else {
            nodes_replace_str(&global->nodes, global->nodes.message_selector, "save failed.");
        }
        return ok;
    } else {
        nodes_replace_str(&global->nodes, global->nodes.message_selector, "command not found.");
        return ok;
    }
}
