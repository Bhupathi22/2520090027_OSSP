#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_INPUT 500
#define MAX_ARGS 50
#define MAX_PATH_LENGTH 4096


/*
 * ============================================================
 * PART 1:
 * PROCESS SYNCHRONIZATION USING waitpid()
 * ============================================================
 */

/*
 * Create a child process and execute the command.
 * The parent waits for the child using waitpid().
 */
void execute_command(char *args[], int argc)
{
    pid_t pid;
    int status;

    if (argc == 0)
    {
        printf("Error: Empty command.\n");
        return;
    }

    printf("\n============================================\n");
    printf("       PROCESS SYNCHRONIZATION\n");
    printf("============================================\n");

    printf("Parent PID: %d\n", getpid());

    /*
     * Create child process
     */
    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    /*
     * Child process
     */
    if (pid == 0)
    {
        printf("\n[CHILD]\n");
        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        printf("Executing: %s\n", args[0]);

        /*
         * Execute command
         */
        execvp(args[0], args);

        /*
         * execvp() returns only when execution fails
         */
        perror("execvp");
        exit(127);
    }

    /*
     * Parent process
     */
    else
    {
        printf("\n[PARENT]\n");
        printf("Created child PID: %d\n", pid);

        printf("Parent is waiting using waitpid()...\n");

        /*
         * Wait specifically for this child
         */
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return;
        }

        /*
         * Check how the child terminated
         */
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);

            printf("\nChild terminated normally.\n");
            printf("Child exit status: %d\n",
                   exit_status);
        }

        else if (WIFSIGNALED(status))
        {
            printf("\nChild was terminated by signal %d.\n",
                   WTERMSIG(status));
        }

        else if (WIFSTOPPED(status))
        {
            printf("\nChild was stopped by signal %d.\n",
                   WSTOPSIG(status));
        }

        printf("Parent continues after child termination.\n");
    }
}


/*
 * ============================================================
 * PART 2:
 * PATH VARIABLE AND COMMAND RESOLUTION
 * ============================================================
 */


/*
 * Check whether a path refers to an executable file.
 */
int is_executable(const char *path)
{
    struct stat file_info;

    /*
     * Check whether file exists
     */
    if (stat(path, &file_info) != 0)
    {
        return 0;
    }

    /*
     * Check that it is a regular file
     */
    if (!S_ISREG(file_info.st_mode))
    {
        return 0;
    }

    /*
     * Check execute permission
     */
    if (access(path, X_OK) != 0)
    {
        return 0;
    }

    return 1;
}


/*
 * Resolve a command manually using PATH.
 */
char *find_command(const char *command)
{
    char *path;
    char *path_copy;
    char *directory;

    static char full_path[MAX_PATH_LENGTH];

    /*
     * If command contains '/',
     * treat it as a direct path.
     */
    if (strchr(command, '/') != NULL)
    {
        if (is_executable(command))
        {
            strcpy(full_path, command);
            return full_path;
        }

        return NULL;
    }

    /*
     * Retrieve PATH environment variable
     */
    path = getenv("PATH");

    if (path == NULL)
    {
        printf("PATH variable is not set.\n");
        return NULL;
    }

    /*
     * Make a copy because strtok() modifies the string.
     */
    path_copy = strdup(path);

    if (path_copy == NULL)
    {
        perror("strdup");
        return NULL;
    }

    /*
     * Split PATH into directories.
     */
    directory = strtok(path_copy, ":");

    while (directory != NULL)
    {
        /*
         * Construct:
         *
         * directory + "/" + command
         */
        snprintf(full_path,
                 sizeof(full_path),
                 "%s/%s",
                 directory,
                 command);

        /*
         * Check whether executable exists
         */
        if (is_executable(full_path))
        {
            free(path_copy);
            return full_path;
        }

        directory = strtok(NULL, ":");
    }

    free(path_copy);

    return NULL;
}


/*
 * Display PATH variable.
 */
void display_path()
{
    char *path = getenv("PATH");

    printf("\n============================================\n");
    printf("             PATH VARIABLE\n");
    printf("============================================\n");

    if (path == NULL)
    {
        printf("PATH is not set.\n");
        return;
    }

    printf("PATH:\n%s\n", path);

    printf("\nSearch directories:\n");

    /*
     * Make a copy before using strtok()
     */
    char *path_copy = strdup(path);

    if (path_copy == NULL)
    {
        perror("strdup");
        return;
    }

    char *directory = strtok(path_copy, ":");

    int count = 1;

    while (directory != NULL)
    {
        printf("%d. %s\n", count, directory);

        count++;

        directory = strtok(NULL, ":");
    }

    free(path_copy);
}


/*
 * Resolve command and display result.
 */
void resolve_command(char *command)
{
    printf("\n============================================\n");
    printf("          COMMAND RESOLUTION\n");
    printf("============================================\n");

    printf("Command: %s\n", command);

    char *result = find_command(command);

    if (result != NULL)
    {
        printf("Executable found:\n");
        printf("%s\n", result);

        printf("Execute permission: YES\n");
        printf("Command resolution successful.\n");
    }

    else
    {
        printf("Executable not found.\n");
        printf("Execute permission: NO / COMMAND MISSING\n");
        printf("Command resolution failed.\n");
    }
}


/*
 * ============================================================
 * MAIN PROGRAM
 * ============================================================
 */

int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    int argc = 0;

    printf("============================================\n");
    printf("     PROCESS + PATH COMMAND RESOLVER\n");
    printf("============================================\n");

    /*
     * Display PATH
     */
    display_path();

    /*
     * Ask user for command
     */
    printf("\nEnter a command to resolve and execute:\n");
    printf("> ");

    if (fgets(input,
              sizeof(input),
              stdin) == NULL)
    {
        return 1;
    }

    /*
     * Remove newline
     */
    input[strcspn(input, "\n")] = '\0';

    /*
     * Check empty input
     */
    if (strlen(input) == 0)
    {
        printf("Error: Empty command.\n");
        return 1;
    }

    /*
     * Split command into arguments
     */
    char *token = strtok(input, " \t");

    while (token != NULL &&
           argc < MAX_ARGS - 1)
    {
        args[argc] = token;
        argc++;

        token = strtok(NULL, " \t");
    }

    args[argc] = NULL;

    /*
     * Resolve command using PATH
     */
    resolve_command(args[0]);

    /*
     * Execute command and synchronize
     * parent and child using waitpid().
     */
    printf("\nStarting command execution...\n");

    execute_command(args, argc);

    printf("\n============================================\n");
    printf("              PROGRAM COMPLETE\n");
    printf("============================================\n");

    return 0;
}
