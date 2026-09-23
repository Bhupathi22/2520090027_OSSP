#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 20
#define MAX_INPUT 500


int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    int argc = 0;

    printf("========================================\n");
    printf("       PROCESS CREATION & EXECUTION\n");
    printf("========================================\n");

    printf("\nEnter a command to execute:\n> ");

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return 1;
    }

    /*
     * Remove newline
     */
    input[strcspn(input, "\n")] = '\0';

    /*
     * Handle empty input
     */
    if (strlen(input) == 0)
    {
        printf("Error: Empty command.\n");
        return 1;
    }

    /*
     * Split input into arguments
     */
    char *token = strtok(input, " ");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc] = token;
        argc++;

        token = strtok(NULL, " ");
    }

    args[argc] = NULL;

    printf("\nParent process PID: %d\n", getpid());

    /*
     * Create child process
     */
    pid_t pid = fork();

    if (pid < 0)
    {
        /*
         * fork() failed
         */
        perror("fork");
        return 1;
    }

    else if (pid == 0)
    {
        /*
         * CHILD PROCESS
         */
        printf("\n--- CHILD PROCESS ---\n");

        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        printf("Executing program: %s\n", args[0]);

        /*
         * Execute the requested command
         */
        execvp(args[0], args);

        /*
         * execvp() returns only if an error occurs
         */
        perror("execvp");

        exit(EXIT_FAILURE);
    }

    else
    {
        /*
         * PARENT PROCESS
         */
        int status;

        printf("\n--- PARENT PROCESS ---\n");

        printf("Created child process with PID: %d\n",
               pid);

        printf("Parent is waiting for child...\n");

        /*
         * Wait for child to finish
         */
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }

        /*
         * Check how child terminated
         */
        if (WIFEXITED(status))
        {
            printf("\nChild exited normally.\n");
            printf("Exit status: %d\n",
                   WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("\nChild terminated by signal.\n");
        }

        printf("Parent process completed.\n");
    }

    printf("\n========================================\n");
    printf("       PROGRAM COMPLETED\n");
    printf("========================================\n");

    return 0;
}
