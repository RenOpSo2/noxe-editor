#include "draw.h"
#include "nodes.h"
#include <unistd.h>

static void draw_clear() {
    write(STDOUT_FILENO, "\x1b[?25l\x1b[H", 10);
}

static void draw_text(struct gap_buffer* gb, uint32_t size_y) {
    int lines = 0;
    int idx = gb->gap_start - 1;
    while (idx >= 0) {
        if (gb->data[idx] == '\n') {
            lines++;
            if (lines > (int)size_y / 3) {
                break;
            }
        }
        idx--;
    }
    uint32_t line_start = idx + 1;

    uint32_t current_line = 0;
    
    for (uint32_t i = line_start; i < gb->gap_start; i++) {
        if (current_line >= size_y) break;
        char ch = gb->data[i];
        if (ch == '\n') {
            write(STDOUT_FILENO, "\x1b[K", 3);
            current_line++;
        }
        write(STDOUT_FILENO, &ch, 1);
    }
    
    if (current_line < size_y) {
        write(STDOUT_FILENO, "\x1b[7m|\x1b[0m", 9);
    }
    
    for (uint32_t i = gb->gap_end; i < gb->capacity; i++) {
        if (current_line >= size_y) break;
        char ch = gb->data[i];
        if (ch == '\n') {
            write(STDOUT_FILENO, "\x1b[K", 3);
            current_line++;
        }
        write(STDOUT_FILENO, &ch, 1);
    }
    
    write(STDOUT_FILENO, "\x1b[K", 3);
    for (; current_line < size_y; current_line++) {
        write(STDOUT_FILENO, "\n\x1b[K", 4);
    }
}

static void draw_info(enum mode mode) {
    if (mode == mode_insert) {
        write(STDOUT_FILENO, "\x1b[48;5;46m\x1b[30m[INSERT_MODE]\x1b[0m", 33);
    } else if (mode == mode_normal) {
        write(STDOUT_FILENO, "\x1b[48;5;39m\x1b[30m[NORMAL_MODE]\x1b[0m", 33);
    } else if (mode == mode_raw) {
        write(STDOUT_FILENO, "\x1b[48;5;226m\x1b[30m[RAW_MODE]\x1b[0m", 30);
    } else if (mode == mode_cmd) {
        write(STDOUT_FILENO, "\x1b[48;5;196m\x1b[37m[CMD_MODE]\x1b[0m", 30);
    }
}

static void draw_message(struct gap_buffer* msg) {
    if (msg->gap_start > 0 || (msg->capacity - msg->gap_end) > 0) {
        write(STDOUT_FILENO, ", message: [", 12);
        
        if (msg->gap_start > 0)
            write(STDOUT_FILENO, msg->data, msg->gap_start);
        
        uint32_t after = msg->capacity - msg->gap_end;
        if (after > 0)
            write(STDOUT_FILENO, msg->data + msg->gap_end, after);
            
        write(STDOUT_FILENO, "]", 1);
    }
}

static void draw_cmd(struct gap_buffer* cmd) {
    if (cmd->gap_start > 0 || (cmd->capacity - cmd->gap_end) > 0) {
        write(STDOUT_FILENO, ", cmd: ", 7);
        if (cmd->gap_start > 0)
            write(STDOUT_FILENO, cmd->data, cmd->gap_start);
        
        uint32_t after = cmd->capacity - cmd->gap_end;
        if (after > 0)
            write(STDOUT_FILENO, cmd->data + cmd->gap_end, after);
    }
}

void draw_update(struct global* global) {
    draw_clear();
    draw_info(global->mode);
    draw_message(&global->msg);
    draw_cmd(&global->cmd);
    write(STDOUT_FILENO, "\x1b[K\n", 4);
    draw_text(&global->text, global->term.ws.ws_row - 2);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void draw_deinit() {
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
}
