/*** includes */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines */
#define CTRL_KEY(k) ((k) & 0x1f)

/*** data */
struct termios originalTermios;

/*** terminal */

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &originalTermios) == -1) {
        die("tcgetattr");
    }

    atexit(disableRawMode);

    struct termios raw = originalTermios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)) {
        die("tcsetattr");
    }
}

void terminalRead() {
}

char editorReadKey() {
    char c;
    int nread;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }

    return c;
}

void editorProcessInput() {
    char c = editorReadKey();

    switch (c) {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }

}

/*** main */
int main() {
    enableRawMode();

    while (1) {
        editorProcessInput();
    }

    return 0;
}
