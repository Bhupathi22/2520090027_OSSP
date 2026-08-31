#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    int pipefd[2];
    pid_t pid;
    char buffer[100];

    char *data = "Hello Child! This data is sent by the Producer.";

    struct timespec start, end;
    double time_taken;

    // Create pipe
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create child process
    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    // Child - Consumer
    if (pid == 0)
    {
        close(pipefd[1]);  // Close write end

        read(pipefd[0], buffer, sizeof(buffer));

        printf("Consumer received: %s\n", buffer);

        close(pipefd[0]);

        exit(0);
    }

    // Parent - Producer
    else
    {
        close(pipefd[0]);  // Close read end

        write(pipefd[1], data, strlen(data) + 1);

        close(pipefd[1]);

        wait(NULL);

        // Stop timer
        clock_gettime(CLOCK_MONOTONIC, &end);

        time_taken = (end.tv_sec - start.tv_sec) * 1e6;
        time_taken += (end.tv_nsec - start.tv_nsec) / 1e3;

        printf("Communication time: %.2f microseconds\n", time_taken);
    }

    return 0;
}
