#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char input[100];
    char *args[20];

    printf("Enter Linux command: ");
    fgets(input, sizeof(input), stdin);

    // Remove newline
    input[strcspn(input, "\n")] = '\0';

    // Split command into arguments
    int i = 0;
    args[i] = strtok(input, " ");

    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed!\n");
        return 1;
    }

    else if (pid == 0) {
        // Child Process
        printf("\n===== CHILD PROCESS =====\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        execvp(args[0], args);

        // Executes only if exec fails
        printf("Invalid command!\n");
        exit(1);
    }

    else {
        // Parent Process
        printf("\n===== PARENT PROCESS =====\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        wait(NULL);

        printf("\nChild process has finished execution.\n");
    }

    return 0;
}
