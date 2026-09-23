#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT 1000
#define MAX_TOKENS 50
#define MAX_TOKEN_SIZE 500
#define MAX_DIRECTORY 4096


/* =========================================================
   TOKEN STRUCTURE
   ========================================================= */

typedef struct
{
    char value[MAX_TOKEN_SIZE];
} Token;


/* =========================================================
   SHELL STATE
   ========================================================= */

typedef struct
{
    int running;
} ShellState;

ShellState shell;


/* =========================================================
   VARIABLE EXPANSION
   ========================================================= */

/*
   Get the value of an environment variable.
   If it does not exist, return an empty string.
*/
const char *get_variable_value(const char *name)
{
    const char *value = getenv(name);

    if (value == NULL)
    {
        return "";
    }

    return value;
}


/*
   Expand:

       $USER
       $HOME
       ${USER}
       ${HOME}
*/
void expand_variables(const char *input, char *output)
{
    int i = 0;
    int j = 0;

    while (input[i] != '\0' &&
           j < MAX_TOKEN_SIZE - 1)
    {
        /*
           Check for $
        */
        if (input[i] == '$')
        {
            char variable_name[100];
            int name_length = 0;

            /*
               ${VARIABLE}
            */
            if (input[i + 1] == '{')
            {
                i += 2;

                while (input[i] != '\0' &&
                       input[i] != '}' &&
                       name_length < 99)
                {
                    variable_name[name_length] = input[i];
                    name_length++;
                    i++;
                }

                variable_name[name_length] = '\0';

                /*
                   Skip closing }
                */
                if (input[i] == '}')
                {
                    i++;
                }
            }

            /*
               $VARIABLE
            */
            else
            {
                i++;

                while (input[i] != '\0' &&
                       (
                           (input[i] >= 'A' &&
                            input[i] <= 'Z') ||

                           (input[i] >= 'a' &&
                            input[i] <= 'z') ||

                           (input[i] >= '0' &&
                            input[i] <= '9') ||

                           input[i] == '_'
                       ) &&
                       name_length < 99)
                {
                    variable_name[name_length] = input[i];
                    name_length++;
                    i++;
                }

                variable_name[name_length] = '\0';

                /*
                   If only $ was found
                */
                if (name_length == 0)
                {
                    output[j] = '$';
                    j++;
                    continue;
                }
            }

            /*
               Get variable value
            */
            const char *value =
                get_variable_value(variable_name);

            /*
               Copy variable value
               into output
            */
            int k = 0;

            while (value[k] != '\0' &&
                   j < MAX_TOKEN_SIZE - 1)
            {
                output[j] = value[k];
                j++;
                k++;
            }
        }
        else
        {
            /*
               Normal character
            */
            output[j] = input[i];
            i++;
            j++;
        }
    }

    output[j] = '\0';
}


/*
   Expand all tokens
*/
void expand_tokens(Token tokens[], int count)
{
    printf("\n========== VARIABLE EXPANSION ==========\n");

    for (int i = 0; i < count; i++)
    {
        char expanded[MAX_TOKEN_SIZE];

        expand_variables(tokens[i].value,
                         expanded);

        printf("Before: %s\n",
               tokens[i].value);

        strcpy(tokens[i].value,
               expanded);

        printf("After : %s\n\n",
               tokens[i].value);
    }
}


/* =========================================================
   BUILT-IN COMMANDS
   ========================================================= */


/*
   cd
*/
void builtin_cd(char *args[], int argc)
{
    const char *directory;

    if (argc == 1)
    {
        directory = getenv("HOME");

        if (directory == NULL)
        {
            printf("cd: HOME is not set\n");
            return;
        }
    }
    else
    {
        directory = args[1];
    }

    if (chdir(directory) != 0)
    {
        perror("cd");
        return;
    }

    printf("Directory changed successfully.\n");
}


/*
   pwd
*/
void builtin_pwd(char *args[], int argc)
{
    char directory[MAX_DIRECTORY];

    (void)args;
    (void)argc;

    if (getcwd(directory,
               sizeof(directory)) == NULL)
    {
        perror("pwd");
        return;
    }

    printf("%s\n", directory);
}


/*
   echo
*/
void builtin_echo(char *args[], int argc)
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s", args[i]);

        if (i < argc - 1)
        {
            printf(" ");
        }
    }

    printf("\n");
}


/*
   export

   Example:

       export NAME=Chaitanya
*/
void builtin_export(char *args[], int argc)
{
    if (argc < 2)
    {
        printf("Usage: export NAME=VALUE\n");
        return;
    }

    char *equal_sign =
        strchr(args[1], '=');

    if (equal_sign == NULL)
    {
        printf("Usage: export NAME=VALUE\n");
        return;
    }

    /*
       Split NAME and VALUE
    */
    *equal_sign = '\0';

    char *name = args[1];
    char *value = equal_sign + 1;

    /*
       Update environment variable
    */
    if (setenv(name, value, 1) != 0)
    {
        perror("export");
        return;
    }

    printf("Variable %s updated successfully.\n",
           name);
}


/*
   env
*/
void builtin_env(char *args[], int argc)
{
    extern char **environ;

    (void)args;
    (void)argc;

    for (char **environment = environ;
         *environment != NULL;
         environment++)
    {
        printf("%s\n", *environment);
    }
}


/*
   exit
*/
void builtin_exit(char *args[], int argc)
{
    (void)args;
    (void)argc;

    printf("Exiting shell...\n");

    shell.running = 0;
}


/* =========================================================
   BUILT-IN DISPATCH TABLE
   ========================================================= */

typedef void (*BuiltinFunction)(char *args[],
                                int argc);


typedef struct
{
    const char *name;
    BuiltinFunction function;
} Builtin;


Builtin builtin_table[] =
{
    {"cd",     builtin_cd},
    {"pwd",    builtin_pwd},
    {"echo",   builtin_echo},
    {"export", builtin_export},
    {"env",    builtin_env},
    {"exit",   builtin_exit}
};


#define BUILTIN_COUNT \
    (sizeof(builtin_table) / sizeof(builtin_table[0]))


/*
   Search dispatch table
*/
BuiltinFunction find_builtin(const char *command)
{
    for (size_t i = 0;
         i < BUILTIN_COUNT;
         i++)
    {
        if (strcmp(command,
                   builtin_table[i].name) == 0)
        {
            return builtin_table[i].function;
        }
    }

    return NULL;
}


/*
   Execute built-in
*/
int execute_builtin(char *args[], int argc)
{
    BuiltinFunction function;

    function = find_builtin(args[0]);

    if (function == NULL)
    {
        return 0;
    }

    printf("\n[BUILT-IN COMMAND]\n");
    printf("Command: %s\n",
           args[0]);

    function(args, argc);

    return 1;
}


/* =========================================================
   TOKENIZATION
   ========================================================= */

int tokenize(char input[],
             Token tokens[])
{
    int count = 0;

    char *token =
        strtok(input, " \t\n");

    while (token != NULL &&
           count < MAX_TOKENS - 1)
    {
        strncpy(tokens[count].value,
                token,
                MAX_TOKEN_SIZE - 1);

        tokens[count].value[
            MAX_TOKEN_SIZE - 1
        ] = '\0';

        count++;

        token =
            strtok(NULL, " \t\n");
    }

    return count;
}


/* =========================================================
   DISPLAY TOKENS
   ========================================================= */

void display_tokens(Token tokens[],
                    int count)
{
    printf("\n========== FINAL TOKENS ==========\n");

    for (int i = 0; i < count; i++)
    {
        printf("Token %d: %s\n",
               i + 1,
               tokens[i].value);
    }

    printf("=================================\n");
}


/* =========================================================
   DISPLAY DISPATCH TABLE
   ========================================================= */

void display_dispatch_table(void)
{
    printf("\n========== BUILT-IN DISPATCH TABLE ==========\n");

    for (size_t i = 0;
         i < BUILTIN_COUNT;
         i++)
    {
        printf("%zu. %s\n",
               i + 1,
               builtin_table[i].name);
    }

    printf("=============================================\n");
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    char input[MAX_INPUT];

    Token tokens[MAX_TOKENS];

    char *args[MAX_TOKENS];

    shell.running = 1;

    printf("============================================\n");
    printf("    VARIABLE EXPANSION + BUILT-IN SHELL\n");
    printf("============================================\n");

    display_dispatch_table();

    printf("\nVariable formats supported:\n");
    printf("  $USER\n");
    printf("  $HOME\n");
    printf("  ${USER}\n");
    printf("  ${HOME}\n");

    printf("\n");

    while (shell.running)
    {
        printf("mini-shell> ");

        if (fgets(input,
                  sizeof(input),
                  stdin) == NULL)
        {
            break;
        }

        /*
           Empty input
        */
        if (input[0] == '\n')
        {
            continue;
        }

        /*
           Tokenize
        */
        int count =
            tokenize(input, tokens);

        if (count == 0)
        {
            continue;
        }

        /*
           Variable expansion
        */
        expand_tokens(tokens, count);

        /*
           Display tokens
        */
        display_tokens(tokens, count);

        /*
           Convert tokens to args
        */
        for (int i = 0;
             i < count;
             i++)
        {
            args[i] =
                tokens[i].value;
        }

        args[count] = NULL;

        /*
           Execute built-in
        */
        if (execute_builtin(args, count))
        {
            continue;
        }

        /*
           Invalid command
        */
        printf("\nInvalid built-in command: %s\n",
               args[0]);

        printf("\nAvailable commands:\n");

        for (size_t i = 0;
             i < BUILTIN_COUNT;
             i++)
        {
            printf("  %s\n",
                   builtin_table[i].name);
        }

        printf("\n");
    }

    printf("\nShell terminated.\n");

    return 0;
}
