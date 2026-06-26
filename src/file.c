#include "file.h"
#include "nodes.h"
#include <fcntl.h>
#include <unistd.h>

enum result file_read(struct nodes* nodes, struct node* dst, const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return err;
    }
    char buffer[1];
    uint32_t bytes_read;
    while ((bytes_read = read(fd, buffer, 1)) > 0) {
        nodes_insert(nodes, dst, buffer[0]);
    }
    if ((int)bytes_read == -1) {
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
    char buffer[1];
    for (; itr->next != NULL; itr = itr->next) {
        buffer[0] = itr->ch;
        if (write(fd, buffer, 1) != 1) {
            close(fd);
            return err;
        }
    }
    close(fd);
    return ok;
}
