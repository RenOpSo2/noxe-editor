#ifndef CMD_H
#define CMD_H

#include "global.h"

enum result cmd_openfile(struct nodes* nodes, const char* path);
enum result cmd_savefile(struct nodes* nodes, const char* path);
enum result cmd_exec(struct global* global, struct node* this);

#endif
