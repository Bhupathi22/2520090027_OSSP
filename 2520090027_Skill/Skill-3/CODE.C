#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_HISTORY 10
#define MAX_LENGTH 100

char history[MAX_HISTORY][MAX_LENGTH];
int history_count = 0;

void disable_raw_mode(struct termios *old) {
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

void enable_raw_mode(struct termios *old) {
    struct termios new;

    tcgetattr(STDIN_FILENO, old);
    new = *old;

    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void show_previous(char *buffer, int *index) {
    if (*index > 0) {
        (*index)--;

        strcpy(buffer, history[*index]);

        printf("\r\033[K$ %s", buffer);
        fflush(stdout);
    }
}

void show_next(char *buffer, int *index) {
    if (*index < history_count - 1) {
        (*index)++;

        strcpy(buffer, history[*index]);

        printf("\r\033[K$ %s", buffer);
        fflush(stdout);
    }
}

int main() {
    struct termios old;
    char buffer[MAX_LENGTH];

    enable_raw_mode(&old);

    printf("Simple Command History\n");
    printf("Type commands. Press UP/DOWN arrows.\n");
    printf("Type exit to quit.\n\n");

    while (1) {
        int pos = 0;
        int history_index = history_count;

        memset(buffer, 0, sizeof(buffer));

        printf("$ ");
        fflush(stdout);

        while (1) {
            char c;
            read(STDIN_FILENO, &c, 1);

            // Enter key
            if (c == '\n' || c == '\r') {
                buffer[pos] = '\0';
                printf("\n");

                if (strcmp(buffer, "exit") == 0) {
                    disable_raw_mode(&old);
                    return 0;
                }

                if (pos > 0 && history_count < MAX_HISTORY) {
                    strcpy(history[history_count], buffer);
                    history_count++;
                }

                break;
            }

            // Backspace
            else if (c == 127) {
                if (pos > 0) {
                    pos--;
                    buffer[pos] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            }

            // Escape sequence
            else if (c == 27) {
                char seq[2];

                read(STDIN_FILENO, &seq[0], 1);
                read(STDIN_FILENO, &seq[1], 1);

                // UP arrow
                if (seq[0] == '[' && seq[1] == 'A') {
                    show_previous(buffer, &history_index);
                    pos = strlen(buffer);
                }

                // DOWN arrow
                else if (seq[0] == '[' && seq[1] == 'B') {
                    show_next(buffer, &history_index);
                    pos = strlen(buffer);
                }
            }

            // Normal character
            else if (c >= 32 && c <= 126) {
                if (pos < MAX_LENGTH - 1) {
                    buffer[pos++] = c;
                    putchar(c);
                    fflush(stdout);
                }
            }
        }
    }

    disable_raw_mode(&old);

    return 0;
}
