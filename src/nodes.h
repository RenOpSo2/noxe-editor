#ifndef NODES_H
#define NODES_H

#include "global.h"

void gb_init(struct gap_buffer* gb, uint32_t initial_capacity, Arena* arena);
void gb_insert(struct gap_buffer* gb, char ch, Arena* arena);
void gb_delete(struct gap_buffer* gb);
void gb_clear(struct gap_buffer* gb);
void gb_insert_str(struct gap_buffer* gb, const char* src, Arena* arena);
void gb_replace_str(struct gap_buffer* gb, const char* src, Arena* arena);
void gb_to_str(char* dst, struct gap_buffer* gb);
void gb_move_left(struct gap_buffer* gb);
void gb_move_right(struct gap_buffer* gb);
void gb_move_up(struct gap_buffer* gb);
void gb_move_down(struct gap_buffer* gb);
uint32_t gb_get_line(struct gap_buffer* gb);
uint32_t gb_get_col(struct gap_buffer* gb);

#endif
