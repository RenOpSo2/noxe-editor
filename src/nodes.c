#include "nodes.h"
#include <string.h>

void gb_init(struct gap_buffer* gb, uint32_t initial_capacity, Arena* arena) {
    if (initial_capacity < 64) initial_capacity = 64;
    gb->data = (char*)arena_alloc_default(arena, initial_capacity);
    gb->capacity = initial_capacity;
    gb->gap_start = 0;
    gb->gap_end = initial_capacity;
}

static void gb_grow(struct gap_buffer* gb, Arena* arena) {
    uint32_t new_cap = gb->capacity * 2;
    char* new_data = (char*)arena_alloc_default(arena, new_cap);
    
    // Copy before gap
    if (gb->gap_start > 0) {
        memcpy(new_data, gb->data, gb->gap_start);
    }
    
    // Copy after gap
    uint32_t after_gap_len = gb->capacity - gb->gap_end;
    uint32_t new_gap_end = new_cap - after_gap_len;
    if (after_gap_len > 0) {
        memcpy(new_data + new_gap_end, gb->data + gb->gap_end, after_gap_len);
    }
    
    gb->data = new_data;
    gb->capacity = new_cap;
    gb->gap_end = new_gap_end;
}

void gb_insert(struct gap_buffer* gb, char ch, Arena* arena) {
    if (gb->gap_start == gb->gap_end) {
        gb_grow(gb, arena);
    }
    gb->data[gb->gap_start++] = ch;
}

void gb_delete(struct gap_buffer* gb) {
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

void gb_clear(struct gap_buffer* gb) {
    gb->gap_start = 0;
    gb->gap_end = gb->capacity;
}

void gb_insert_str(struct gap_buffer* gb, const char* src, Arena* arena) {
    for (uint32_t i = 0; src[i] != '\0'; i++) {
        gb_insert(gb, src[i], arena);
    }
}

void gb_replace_str(struct gap_buffer* gb, const char* src, Arena* arena) {
    gb_clear(gb);
    gb_insert_str(gb, src, arena);
}

void gb_to_str(char* dst, struct gap_buffer* gb) {
    uint32_t i = 0;
    for (uint32_t j = 0; j < gb->gap_start; j++) {
        dst[i++] = gb->data[j];
    }
    for (uint32_t j = gb->gap_end; j < gb->capacity; j++) {
        dst[i++] = gb->data[j];
    }
    dst[i] = '\0';
}

void gb_move_left(struct gap_buffer* gb) {
    if (gb->gap_start > 0) {
        gb->gap_end--;
        gb->gap_start--;
        gb->data[gb->gap_end] = gb->data[gb->gap_start];
    }
}

void gb_move_right(struct gap_buffer* gb) {
    if (gb->gap_end < gb->capacity) {
        gb->data[gb->gap_start] = gb->data[gb->gap_end];
        gb->gap_start++;
        gb->gap_end++;
    }
}


void gb_move_up(struct gap_buffer* gb) {
    // Current column
    uint32_t col = 0;
    for (int i = gb->gap_start - 1; i >= 0; i--) {
        if (gb->data[i] == '\n') break;
        col++;
    }
    
    // Find previous newline
    int prev_nl = -1;
    for (int i = gb->gap_start - 1; i >= 0; i--) {
        if (gb->data[i] == '\n') {
            prev_nl = i;
            break;
        }
    }
    
    if (prev_nl == -1) return; // Already on first line
    
    // Find start of the line above
    int start_of_prev = 0;
    for (int i = prev_nl - 1; i >= 0; i--) {
        if (gb->data[i] == '\n') {
            start_of_prev = i + 1;
            break;
        }
    }
    
    // Target position
    int target = start_of_prev + col;
    if (target > prev_nl) target = prev_nl;
    
    while (gb->gap_start > (uint32_t)target) {
        gb_move_left(gb);
    }
}

void gb_move_down(struct gap_buffer* gb) {
    // Current column
    uint32_t col = 0;
    for (int i = gb->gap_start - 1; i >= 0; i--) {
        if (gb->data[i] == '\n') break;
        col++;
    }
    
    // Find next newline
    int next_nl = -1;
    for (uint32_t i = gb->gap_end; i < gb->capacity; i++) {
        if (gb->data[i] == '\n') {
            next_nl = i;
            break;
        }
    }
    
    if (next_nl == -1) {
        // Move to end of buffer
        while (gb->gap_end < gb->capacity) gb_move_right(gb);
        return;
    }
    
    // Find end of the line below
    int end_of_next = gb->capacity;
    for (uint32_t i = next_nl + 1; i < gb->capacity; i++) {
        if (gb->data[i] == '\n') {
            end_of_next = i;
            break;
        }
    }
    
    int target = next_nl + 1 + col;
    if (target > end_of_next) target = end_of_next;
    
    // Move right until we reach the target
    // Note target is calculated as index in the full capacity array assuming gap is empty.
    // Distance to move right: target - next_nl + (distance to next_nl)
    // Actually, simply count right moves.
    int target_gap_start = gb->gap_start + (target - gb->gap_end);
    while (gb->gap_start < (uint32_t)target_gap_start) {
        gb_move_right(gb);
    }
}
