#include "nodes.h"

void nodes_free(struct nodes* nodes, struct node* this) {
    this->prev = nodes->passive_selector;
    nodes->passive_selector->next = this;
    nodes->passive_selector = this;
}

struct node* nodes_allocate(struct nodes* nodes) {
    struct node* this = nodes->passive_selector;
    nodes->passive_selector = nodes->passive_selector->prev;
    return this;
}

struct node* nodes_insert(struct nodes* nodes, struct node* next, char ch) {
    struct node* this = nodes_allocate(nodes);
    struct node* prev = next->prev;
    this->ch = ch;
    this->next = next;
    this->prev = prev;
    next->prev = this;
    if (prev != NULL) {
        prev->next = this;
    }
    return this;
}

void nodes_delete(struct nodes* nodes, struct node* this) {
    struct node* next = this->next;
    struct node* prev = this->prev;
    nodes_free(nodes, this);
    if (next != NULL) {
        next->prev = prev;
    }
    if (prev != NULL) {
        prev->next = next;
    }
}

void nodes_clear(struct nodes* nodes, struct node* this) {
    struct node* itr = this;
    while (itr->next != NULL) {
        nodes_delete(nodes, itr->next);
    }
    while (itr->prev != NULL) {
        nodes_delete(nodes, itr->prev);
    }
    itr->ch = '\0';
}

void nodes_insert_str(struct nodes* nodes, struct node* next, const char* src) {
    for (uint32_t i = 0; src[i] != '\0'; i++) {
        nodes_insert(nodes, next, src[i]);
    }
}

void nodes_replace_str(struct nodes* nodes, struct node* this, const char* src) {
    nodes_clear(nodes, this);
    nodes_insert_str(nodes, this, src);
}

uint32_t nodes_line_left(struct node* this) {
    struct node* itr = this->prev;
    uint32_t i = 0;
    while (itr != NULL && itr->ch != '\n') {
        itr = itr->prev;
        i++;
    }
    return i;
}

struct node* nodes_line_begin(struct node* this) {
    struct node* itr = this;
    while (itr->prev != NULL) {
        if (itr->prev->ch == '\n') {
            break;
        }
        itr = itr->prev;
    }
    return itr;
}

struct node* nodes_line_rbegin(struct node* this) {
    struct node* itr = this;
    while (itr->next != NULL && itr->ch != '\n') {
        itr = itr->next;
    }
    return itr;
}

void nodes_to_str(char* dst, struct node* src) {
    struct node* itr = src;
    uint32_t i;
    while (itr->prev != NULL) {
        itr = itr->prev;
    }
    for (i = 0; itr != NULL; i++) {
        dst[i] = itr->ch;
        itr = itr->next;
    }
    dst[i + 1] = '\0';
}

void nodes_init(struct nodes* nodes) {
    nodes->passive_selector = nodes->data;
    for (uint32_t i = 0; i < nodes_capacity - 1; i++) {
        nodes_free(nodes, &nodes->data[i + 1]);
    }
    nodes->insert_selector = nodes_allocate(nodes);
    nodes->insert_selector->ch = '\0';
    nodes->insert_selector->prev = NULL;
    nodes->insert_selector->next = NULL;
    nodes->cmd_selector = nodes_allocate(nodes);
    nodes->cmd_selector->ch = '\0';
    nodes->cmd_selector->prev = NULL;
    nodes->cmd_selector->next = NULL;
    nodes->message_selector = nodes_allocate(nodes);
    nodes->message_selector->ch = '\0';
    nodes->message_selector->prev = NULL;
    nodes->message_selector->next = NULL;
}
