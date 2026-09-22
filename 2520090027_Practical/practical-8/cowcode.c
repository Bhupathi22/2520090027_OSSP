#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define SIZE (50 * 1024 * 1024)

void print_memory(const char *process_name) {

    char filename[100];

    snprintf(filename,
             sizeof(filename),
             "/proc/%d/status",
             getpid());

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    char line[256];

    printf("\n--- Memory information: %s (PID %d) ---\n",
           process_name,
           getpid());

    while (fgets(line, sizeof(line), file)) {

        if (strncmp(line, "VmSize:", 7) == 0 ||
            strncmp(line, "VmRSS:", 6) == 0 ||
            strncmp(line, "RssAnon:", 8) == 0 ||
            strncmp(line, "RssFile:", 8) == 0) {

            printf("%s", line);
        }
    }

    fclose(file);
}

int main() {

    printf("============================================\n");
    printf("       COPY-ON-WRITE DEMONSTRATION\n");
    printf("============================================\n");

    /* Allocate 50 MB */
    char *memory = malloc(SIZE);

    if (memory == NULL) {
        perror("malloc");
        return 1;
    }

    /*
     * Touch every page so that the memory is actually
     * allocated and visible in the process memory.
     */
    for (size_t i = 0; i < SIZE; i += 4096) {
        memory[i] = 'A';
    }

    printf("\nParent allocated 50 MB.\n");

    print_memory("PARENT - BEFORE FORK");

    pid_t pid = fork();

    if (pid < 0) {

        perror("fork");
        free(memory);
        return 1;

    }

    else if (pid == 0) {

        /*
         * Child process
         */
        printf("\n============================================\n");
        printf("CHILD PROCESS\n");
        printf("============================================\n");

        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        print_memory("CHILD - AFTER FORK");

        printf("\nChild is modifying the memory...\n");

        /*
         * Modify every page.
         *
         * This causes Copy-on-Write.
         */
        for (size_t i = 0; i < SIZE; i += 4096) {
            memory[i] = 'B';
        }

        print_memory("CHILD - AFTER MODIFICATION");

        printf("\nChild memory[0] = %c\n",
               memory[0]);

        free(memory);

        printf("\nChild exiting...\n");

        exit(0);
    }

    else {

        /*
         * Parent process
         */

        printf("\n============================================\n");
        printf("PARENT PROCESS\n");
        printf("============================================\n");

        printf("Parent PID: %d\n", getpid());
        printf("Child PID : %d\n", pid);

        sleep(1);

        print_memory("PARENT - AFTER CHILD MODIFICATION");

        printf("\nParent memory[0] = %c\n",
               memory[0]);

        wait(NULL);

        printf("\nChild process completed.\n");

        free(memory);

        printf("Parent memory released.\n");
    }

    printf("\n============================================\n");
    printf("        COW DEMONSTRATION COMPLETE\n");
    printf("============================================\n");

    return 0;
}
