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

#define ALT_SCREEN_ON  "\x1b[?1049h"  /* switch to alternate screen bu
ffer */
#define RESET_ATTR     "\x1b[0m"      /* reset all text attributes */ 
#define FG_WHITE       "\x1b[37m"     /* set foreground color to white
 */



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


void term_update(struct term* term) {
    ioctl(STDIN_FILENO, TIOCGWINSZ, &term->ws);
}


void term_deinit() {
    write(STDOUT_FILENO, "\x1b[0m", 4);
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    struct termios term;
    ioctl(STDIN_FILENO, TCGETS, &term);
    term.c_lflag |= (ICANON | ECHO);
    ioctl(STDIN_FILENO, TCSETS, &term);
}


void term_init(void) {
    struct termios term;

    /* get current terminal attributes */
    if (ioctl(STDIN_FILENO, TCGETS, &term) == -1)
        return;

    /* disable canonical mode and echo for raw input */
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN]  = 0;   /* read() returns immediately, even with no bytes */
    term.c_cc[VTIME] = 1;   /* 100ms timeout for read() */

    /* apply modified terminal settings */
    ioctl(STDIN_FILENO, TCSETS, &term);

    /* enter alternate screen, reset colors, set default foreground */
    write(STDOUT_FILENO, ALT_SCREEN_ON, 8);
    write(STDOUT_FILENO, RESET_ATTR, 4);
    write(STDOUT_FILENO, FG_WHITE, 5);
}
