#include "nodes.h"
#include <string.h>

static struct page* page_new(Arena* arena) {
    struct page* p = arena_cnew(arena, struct page);
    p->gap_start = 0;
    p->gap_end = PAGE_CAPACITY;
    p->next = NULL;
    p->prev = NULL;
    return p;
}

void pgb_init(struct paged_gap_buffer* pgb, Arena* arena) {
    pgb->head = page_new(arena);
    pgb->tail = pgb->head;
    pgb->active_page = pgb->head;
}

static void page_split(struct paged_gap_buffer* pgb, Arena* arena) {
    struct page* curr = pgb->active_page;
    struct page* new_page = page_new(arena);

    uint32_t right_len = PAGE_CAPACITY - curr->gap_end;
    new_page->gap_end = PAGE_CAPACITY - right_len;

    if (right_len > 0) {
        memcpy(new_page->data + new_page->gap_end, curr->data + curr->gap_end, right_len);
    }

    curr->gap_end = PAGE_CAPACITY;

    new_page->prev = curr;
    new_page->next = curr->next;
    if (curr->next) curr->next->prev = new_page;
    else pgb->tail = new_page;
    curr->next = new_page;
}

void pgb_insert(struct paged_gap_buffer* pgb, char ch, Arena* arena) {
    struct page* p = pgb->active_page;
    if (p->gap_start == p->gap_end) {
        page_split(pgb, arena);
        p = pgb->active_page;
    }
    p->data[p->gap_start++] = ch;
}

void pgb_delete(struct paged_gap_buffer* pgb) {
    struct page* p = pgb->active_page;
    if (p->gap_start > 0) {
        p->gap_start--;
    } else if (p->prev) {
        pgb->active_page = p->prev;
        pgb_delete(pgb);
    }
}

void pgb_clear(struct paged_gap_buffer* pgb) {
    struct page* p = pgb->head;
    while (p) {
        p->gap_start = 0;
        p->gap_end = PAGE_CAPACITY;
        p = p->next;
    }
    pgb->active_page = pgb->head;
}

void pgb_insert_str(struct paged_gap_buffer* pgb, const char* src, Arena* arena) {
    for (uint32_t i = 0; src[i] != '\0'; i++) {
        pgb_insert(pgb, src[i], arena);
    }
}

void pgb_replace_str(struct paged_gap_buffer* pgb, const char* src, Arena* arena) {
    pgb_clear(pgb);
    pgb_insert_str(pgb, src, arena);
}

void pgb_to_str(char* dst, struct paged_gap_buffer* pgb) {
    uint32_t i = 0;
    struct page* p = pgb->head;
    while (p) {
        for (uint32_t j = 0; j < p->gap_start; j++) dst[i++] = p->data[j];
        for (uint32_t j = p->gap_end; j < PAGE_CAPACITY; j++) dst[i++] = p->data[j];
        p = p->next;
    }
    dst[i] = '\0';
}

void pgb_move_left(struct paged_gap_buffer* pgb) {
    struct page* p = pgb->active_page;
    if (p->gap_start > 0) {
        p->gap_end--;
        p->gap_start--;
        p->data[p->gap_end] = p->data[p->gap_start];
    } else if (p->prev) {
        pgb->active_page = p->prev;
        p = pgb->active_page;
        while (p->gap_end < PAGE_CAPACITY) {
            p->data[p->gap_start++] = p->data[p->gap_end++];
        }
        if (p->gap_start > 0) {
            p->gap_end--;
            p->gap_start--;
            p->data[p->gap_end] = p->data[p->gap_start];
        }
    }
}

void pgb_move_right(struct paged_gap_buffer* pgb) {
    struct page* p = pgb->active_page;
    if (p->gap_end < PAGE_CAPACITY) {
        p->data[p->gap_start++] = p->data[p->gap_end++];
    } else if (p->next) {
        pgb->active_page = p->next;
        p = pgb->active_page;
        while (p->gap_start > 0) {
            p->gap_end--;
            p->gap_start--;
            p->data[p->gap_end] = p->data[p->gap_start];
        }
        if (p->gap_end < PAGE_CAPACITY) {
            p->data[p->gap_start++] = p->data[p->gap_end++];
        }
    }
}

void pgb_move_up(struct paged_gap_buffer* pgb) {
    uint32_t col = 0;
    while (1) {
        struct page* p = pgb->active_page;
        if (p->gap_start == 0 && !p->prev) break;
        pgb_move_left(pgb);
        p = pgb->active_page;
        if (p->data[p->gap_start] == '\n') {
            pgb_move_right(pgb);
            break;
        }
        col++;
    }
    if (pgb->active_page->gap_start == 0 && !pgb->active_page->prev) {
        while (col > 0) {
            pgb_move_right(pgb);
            col--;
        }
        return;
    }
    pgb_move_left(pgb);
    uint32_t prev_line_len = 0;
    while (1) {
        struct page* p = pgb->active_page;
        if (p->gap_start == 0 && !p->prev) break;
        pgb_move_left(pgb);
        p = pgb->active_page;
        if (p->data[p->gap_start] == '\n') {
            pgb_move_right(pgb);
            break;
        }
        prev_line_len++;
    }
    uint32_t target = col < prev_line_len ? col : prev_line_len;
    for (uint32_t i = 0; i < target; i++) {
        pgb_move_right(pgb);
    }
}

void pgb_move_down(struct paged_gap_buffer* pgb) {
    uint32_t col = 0;
    while (1) {
        struct page* p = pgb->active_page;
        if (p->gap_start == 0 && !p->prev) break;
        pgb_move_left(pgb);
        p = pgb->active_page;
        if (p->data[p->gap_start] == '\n') {
            pgb_move_right(pgb);
            break;
        }
        col++;
    }
    for (uint32_t i = 0; i < col; i++) {
        pgb_move_right(pgb);
    }
    while (1) {
        struct page* p = pgb->active_page;
        if (p->gap_end == PAGE_CAPACITY && !p->next) return;
        pgb_move_right(pgb);
        p = pgb->active_page;
        if (p->data[p->gap_start - 1] == '\n') {
            break;
        }
    }
    for (uint32_t i = 0; i < col; i++) {
        struct page* p = pgb->active_page;
        if (p->gap_end == PAGE_CAPACITY && !p->next) break;
        pgb_move_right(pgb);
        p = pgb->active_page;
        if (p->data[p->gap_start - 1] == '\n') {
            pgb_move_left(pgb);
            break;
        }
    }
}

// --- Selection & clipboard helpers ---

// Returns the cursor's current linear byte offset from the start of the buffer.
uint32_t pgb_cursor_pos(struct paged_gap_buffer* pgb) {
    uint32_t pos = 0;
    struct page* p = pgb->head;
    while (p) {
        if (p == pgb->active_page) {
            pos += p->gap_start;
            return pos;
        }
        pos += p->gap_start + (PAGE_CAPACITY - p->gap_end);
        p = p->next;
    }
    return pos;
}

// Move cursor to a specific linear byte offset.
void pgb_move_to_pos(struct paged_gap_buffer* pgb, uint32_t target) {
    // Move to start first.
    while (pgb->active_page->prev) {
        pgb->active_page = pgb->active_page->prev;
    }
    while (pgb->active_page->gap_start > 0) {
        pgb->active_page->gap_end--;
        pgb->active_page->gap_start--;
        pgb->active_page->data[pgb->active_page->gap_end] =
            pgb->active_page->data[pgb->active_page->gap_start];
    }
    // Advance right by target steps.
    for (uint32_t i = 0; i < target; i++) {
        struct page* p = pgb->active_page;
        if (p->gap_end == PAGE_CAPACITY && !p->next) break;
        pgb_move_right(pgb);
    }
}

// Copy logical bytes [from, to) from src into dst clipboard.
void pgb_copy_range(struct paged_gap_buffer* dst, struct paged_gap_buffer* src,
                    uint32_t from, uint32_t to, Arena* arena) {
    pgb_clear(dst);
    if (from >= to) return;

    uint32_t pos = 0;
    struct page* p = src->head;

    while (p && pos < to) {
        // Before-gap section of this page
        for (uint32_t i = 0; i < p->gap_start && pos < to; i++, pos++) {
            if (pos >= from) pgb_insert(dst, p->data[i], arena);
        }
        // After-gap section
        for (uint32_t i = p->gap_end; i < PAGE_CAPACITY && pos < to; i++, pos++) {
            if (pos >= from) pgb_insert(dst, p->data[i], arena);
        }
        p = p->next;
    }
}

// Delete logical bytes [from, to) from pgb.
void pgb_delete_range(struct paged_gap_buffer* pgb, uint32_t from, uint32_t to) {
    if (from >= to) return;
    pgb_move_to_pos(pgb, to);
    uint32_t count = to - from;
    for (uint32_t i = 0; i < count; i++) {
        pgb_delete(pgb);
    }
}
