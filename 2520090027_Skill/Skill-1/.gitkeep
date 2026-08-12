#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    printf("Before fork()\n");
    printf("Current Process ID: %d\n\n", getpid());

    pid = fork();

    if (pid < 0)
    {
        printf("Fork failed!\n");
        return 1;
    }
    else if (pid == 0)
    {
        // Child Process
        printf("----- Child Process -----\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        printf("\nExecuting 'ls -l' using exec()...\n\n");

        execl("/bin/ls", "ls", "-l", NULL);

        // Executes only if exec() fails
        perror("exec failed");
        exit(1);
    }
    else
    {
        // Parent Process
        printf("----- Parent Process -----\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        wait(NULL);

        printf("\nChild process completed.\n");
    }

    return 0;
}
