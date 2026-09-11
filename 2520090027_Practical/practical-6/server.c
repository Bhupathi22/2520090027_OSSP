#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define REQUEST_FIFO "/tmp/request_fifo"
#define RESPONSE_FIFO "/tmp/response_fifo"
#define BUFFER_SIZE 256

int main()
{
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Create named pipes
    if (mkfifo(REQUEST_FIFO, 0666) == -1)
    {
        // FIFO may already exist
    }

    if (mkfifo(RESPONSE_FIFO, 0666) == -1)
    {
        // FIFO may already exist
    }

    printf("Server started.\n");
    printf("Waiting for client messages...\n");

    while (1)
    {
        // Open request FIFO for reading
        int request_fd = open(REQUEST_FIFO, O_RDONLY);

        if (request_fd == -1)
        {
            perror("Error opening request FIFO");
            return 1;
        }

        // Read message from client
        ssize_t bytes_read = read(request_fd, buffer, BUFFER_SIZE - 1);
        close(request_fd);

        if (bytes_read <= 0)
        {
            continue;
        }

        buffer[bytes_read] = '\0';

        printf("Client message: %s\n", buffer);

        // Exit command
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Server shutting down...\n");
            break;
        }

        // Process message
        snprintf(response, BUFFER_SIZE,
                 "Server processed: %.230s", buffer);
                 
        int response_fd = open(RESPONSE_FIFO, O_WRONLY);

        if (response_fd == -1)
        {
            perror("Error opening response FIFO");
            return 1;
        }

        // Send response
        write(response_fd, response, strlen(response) + 1);

        close(response_fd);

        printf("Response sent: %s\n", response);
    }

    unlink(REQUEST_FIFO);
    unlink(RESPONSE_FIFO);

    return 0;
}
