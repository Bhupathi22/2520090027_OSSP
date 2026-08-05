#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int source, destination;
    char buffer[1024];
    int bytesRead;

    // Open source file
    source = open("source.txt", O_RDONLY);

    if (source == -1)
    {
        printf("Error opening source file.\n");
        return 1;
    }

    // Create destination file
    destination = open("destination.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (destination == -1)
    {
        printf("Error creating destination file.\n");
        close(source);
        return 1;
    }

    // Copy data
    while ((bytesRead = read(source, buffer, sizeof(buffer))) > 0)
    {
        write(destination, buffer, bytesRead);
    }

    // Close files
    close(source);
    close(destination);

    printf("File copied successfully.\n");

    return 0;
}
