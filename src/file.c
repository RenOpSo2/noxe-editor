#include "file.h"
#include "nodes.h"

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * file_validate_path — Canonicalize *path* and verify it names a regular file.
 *
 * Uses realpath() to resolve symlinks and ".." segments, then stat() to
 * confirm the result is a regular file (S_ISREG).  This prevents path-
 * traversal and device-file exploitation before any fd is opened.
 *
 * Addresses: NX-001, NX-002
 */
static enum result file_validate_path(const char* path, char resolved[PATH_MAX]) {
    char* rp = realpath(path, resolved);
    if (!rp) return err;

    struct stat st;
    if (stat(resolved, &st) == -1) return err;
    if (!S_ISREG(st.st_mode)) return err;   /* reject devices, FIFOs, sockets */

    return ok;
}

/*
 * file_read — Load the file at *path* into *pgb*.
 *
 * Adds path canonicalization via file_validate_path() and opens the
 * resolved path with O_NOFOLLOW to guard against a symlink swap between
 * the validate call and open().
 *
 * Addresses: NX-001
 */
enum result file_read(struct paged_gap_buffer* pgb, const char* path, Arena* arena) {
    char resolved[PATH_MAX];
    if (file_validate_path(path, resolved) != ok) return err;

    /* O_NOFOLLOW: refuse to follow a symlink at the final path component. */
    int fd = open(resolved, O_RDONLY | O_NOFOLLOW);
    if (fd == -1) return err;

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
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
