#ifndef NODES_H
#define NODES_H

#include "global.h"

void pgb_init(struct paged_gap_buffer* pgb, Arena* arena);
void pgb_insert(struct paged_gap_buffer* pgb, char ch, Arena* arena);
void pgb_delete(struct paged_gap_buffer* pgb);
void pgb_clear(struct paged_gap_buffer* pgb);
void pgb_insert_str(struct paged_gap_buffer* pgb, const char* src, Arena* arena);
void pgb_replace_str(struct paged_gap_buffer* pgb, const char* src, Arena* arena);
void pgb_to_str(char* dst, struct paged_gap_buffer* pgb);
void pgb_move_left(struct paged_gap_buffer* pgb);
void pgb_move_right(struct paged_gap_buffer* pgb);
void pgb_move_up(struct paged_gap_buffer* pgb);
void pgb_move_down(struct paged_gap_buffer* pgb);

#endif
