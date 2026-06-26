#ifndef FILE_H
#define FILE_H

#include "global.h"

enum result file_read(struct nodes* nodes, struct node* dst, const char* path);
enum result file_write(const char* path, struct node* src);

#endif
