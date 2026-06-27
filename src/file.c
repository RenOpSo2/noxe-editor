#include "file.h"
#include "nodes.h"
#include <fcntl.h>
#include <unistd.h>

enum result file_read(struct gap_buffer* gb, const char* path, Arena* arena) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return err;
    }
    char buffer[4096];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            gb_insert(gb, buffer[i], arena);
        }
    }
    close(fd);
    if (bytes_read == -1) {
        return err;
    }
    return ok;
}

enum result file_write(const char* path, struct gap_buffer* gb) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        return err;
    }
    
    if (gb->gap_start > 0) {
        if (write(fd, gb->data, gb->gap_start) != (ssize_t)gb->gap_start) {
            close(fd);
            return err;
        }
    }
    
    uint32_t after_len = gb->capacity - gb->gap_end;
    if (after_len > 0) {
        if (write(fd, gb->data + gb->gap_end, after_len) != (ssize_t)after_len) {
            close(fd);
            return err;
        }
    }
    
    close(fd);
    return ok;
}
