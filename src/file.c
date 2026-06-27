#include "file.h"
#include "nodes.h"
#include <fcntl.h>
#include <unistd.h>

enum result file_read(struct paged_gap_buffer* pgb, const char* path, Arena* arena) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) return err;
    char buffer[4096];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            pgb_insert(pgb, buffer[i], arena);
        }
    }
    close(fd);
    if (bytes_read == -1) return err;
    return ok;
}

enum result file_write(const char* path, struct paged_gap_buffer* pgb) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) return err;
    struct page* p = pgb->head;
    while (p) {
        if (p->gap_start > 0) {
            if (write(fd, p->data, p->gap_start) != (ssize_t)p->gap_start) {
                close(fd); return err;
            }
        }
        uint32_t right = PAGE_CAPACITY - p->gap_end;
        if (right > 0) {
            if (write(fd, p->data + p->gap_end, right) != (ssize_t)right) {
                close(fd); return err;
            }
        }
        p = p->next;
    }
    close(fd);
    return ok;
}
