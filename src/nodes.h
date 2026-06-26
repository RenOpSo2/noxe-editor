#ifndef NODES_H
#define NODES_H

#include "global.h"

void nodes_init(struct nodes* nodes);
struct node* nodes_allocate(struct nodes* nodes);
void nodes_free(struct nodes* nodes, struct node* this);
struct node* nodes_insert(struct nodes* nodes, struct node* next, char ch);
void nodes_delete(struct nodes* nodes, struct node* this);
void nodes_clear(struct nodes* nodes, struct node* this);
void nodes_insert_str(struct nodes* nodes, struct node* next, const char* src);
void nodes_replace_str(struct nodes* nodes, struct node* this, const char* src);
uint32_t nodes_line_left(struct node* this);
struct node* nodes_line_begin(struct node* this);
struct node* nodes_line_rbegin(struct node* this);
void nodes_to_str(char* dst, struct node* src);

#endif
