#include "term.h"
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ICANON 0000002
#define ECHO 0000010
#define VMIN 6
#define VTIME 5
#define TCGETS 0x5401
#define TCSETS 0x5402

struct termios {
    unsigned int c_iflag;
    unsigned int c_oflag;
    unsigned int c_cflag;
    unsigned int c_lflag;
    unsigned char c_line;
    unsigned char c_cc[32];
};

uint32_t term_read(char* dst) {
    return read(STDIN_FILENO, dst, term_capacity);
}

void term_deinit() {
    struct termios term;
    ioctl(STDIN_FILENO, TCGETS, &term);
    term.c_lflag |= (ICANON | ECHO);
    ioctl(STDIN_FILENO, TCSETS, &term);
}

void term_update(struct term* term) {
    ioctl(STDIN_FILENO, TIOCGWINSZ, &term->ws);
}

void term_init() {
    struct termios term;
    ioctl(STDIN_FILENO, TCGETS, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    ioctl(STDIN_FILENO, TCSETS, &term);
}
