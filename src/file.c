#include "file.h"
#include "nodes.h"
#include <fcntl.h>
#include <unistd.h>

enum result file_read(struct nodes* nodes, struct node* dst, const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return err;
    }
    char buffer[4096];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            nodes_insert(nodes, dst, buffer[i]);
        }
    }
    if (bytes_read == -1) {
        close(fd);
        return err;
    }
    close(fd);
    return ok;
}

enum result file_write(const char* path, struct node* src) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        return err;
    }
    struct node* itr = src;
    while (itr->prev != NULL) {
        itr = itr->prev;
    }
    char buffer[4096];
    int i = 0;
    for (; itr->next != NULL; itr = itr->next) {
        buffer[i++] = itr->ch;
        if (i == sizeof(buffer)) {
            if (write(fd, buffer, i) != i) {
                close(fd);
                return err;
            }
            i = 0;
        }
    }
    if (i > 0) {
        if (write(fd, buffer, i) != i) {
            close(fd);
            return err;
        }
    }
    close(fd);
    return ok;
}
