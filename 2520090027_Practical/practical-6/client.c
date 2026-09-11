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
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    printf("Client started.\n");

    while (1)
    {
        // Get message from user
        printf("Enter message: ");
        fflush(stdout);

        if (fgets(message, BUFFER_SIZE, stdin) == NULL)
        {
            break;
        }

        // Remove newline character
        message[strcspn(message, "\n")] = '\0';

        // Open request FIFO for writing
        int request_fd = open(REQUEST_FIFO, O_WRONLY);

        if (request_fd == -1)
        {
            perror("Error opening request FIFO");
            return 1;
        }

        // Send message to server
        write(request_fd, message, strlen(message) + 1);

        close(request_fd);

        // Stop client if exit is entered
        if (strcmp(message, "exit") == 0)
        {
            printf("Client exiting...\n");
            break;
        }

        // Open response FIFO for reading
        int response_fd = open(RESPONSE_FIFO, O_RDONLY);

        if (response_fd == -1)
        {
            perror("Error opening response FIFO");
            return 1;
        }

        // Read server response
        ssize_t bytes_read = read(response_fd, response, BUFFER_SIZE - 1);

        close(response_fd);

        if (bytes_read > 0)
        {
            response[bytes_read] = '\0';
            printf("Server response: %s\n", response);
        }
    }

    return 0;
}
