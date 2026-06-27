#ifndef FILE_H
#define FILE_H
#include "global.h"
enum result file_read(struct gap_buffer* gb, const char* path, Arena* arena);
enum result file_write(const char* path, struct gap_buffer* gb);
#endif
