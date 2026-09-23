#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1000
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 200

typedef struct {
    char value[MAX_TOKEN_LENGTH];
    int escaped;
} Token;


/* Add a character to the current token */
void add_character(char token[], int *position, char ch)
{
    if (*position < MAX_TOKEN_LENGTH - 1)
    {
        token[*position] = ch;
        (*position)++;
        token[*position] = '\0';
    }
}


/* Tokenize input while handling escape sequences */
int tokenize(char input[], Token tokens[])
{
    int token_count = 0;
    int i = 0;

    while (input[i] != '\0' && input[i] != '\n')
    {
        /* Skip normal whitespace */
        while (isspace((unsigned char)input[i]))
        {
            i++;
        }

        if (input[i] == '\0' || input[i] == '\n')
            break;

        char token[MAX_TOKEN_LENGTH];
        int position = 0;
        int was_escaped = 0;

        token[0] = '\0';

        while (input[i] != '\0' &&
               input[i] != '\n' &&
               !isspace((unsigned char)input[i]))
        {
            /*
             * Escape character
             */
            if (input[i] == '\\')
            {
                was_escaped = 1;
                i++;

                /*
                 * Check whether '\' is the last character
                 */
                if (input[i] == '\0' ||
                    input[i] == '\n')
                {
                    printf("Error: Incomplete escape sequence.\n");
                    return -1;
                }

                /*
                 * Preserve the character following '\'
                 */
                add_character(token,
                              &position,
                              input[i]);

                i++;
            }
            else
            {
                /*
                 * Normal character
                 */
                add_character(token,
                              &position,
                              input[i]);

                i++;
            }
        }

        if (position > 0)
        {
            strcpy(tokens[token_count].value, token);
            tokens[token_count].escaped = was_escaped;

            token_count++;

            if (token_count >= MAX_TOKENS)
            {
                printf("Error: Too many tokens.\n");
                return -1;
            }
        }
    }

    return token_count;
}


/* Display parsed tokens */
void display_tokens(Token tokens[], int count)
{
    printf("\n========================================\n");
    printf("           PARSER OUTPUT\n");
    printf("========================================\n");

    if (count == 0)
    {
        printf("No tokens found.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        printf("Token %d: \"%s\"",
               i + 1,
               tokens[i].value);

        if (tokens[i].escaped)
            printf("  [ESCAPED]");

        printf("\n");
    }

    printf("========================================\n");
}


/* Validate parser result */
void validate_output(Token tokens[], int count)
{
    printf("\n========================================\n");
    printf("           VALIDATION\n");
    printf("========================================\n");

    if (count == 0)
    {
        printf("Result: Empty input.\n");
        return;
    }

    printf("Parsing successful.\n");
    printf("Total tokens: %d\n", count);

    for (int i = 0; i < count; i++)
    {
        if (tokens[i].escaped)
        {
            printf("Token %d contains preserved escaped characters.\n",
                   i + 1);
        }
    }

    printf("Parser validation completed successfully.\n");
    printf("========================================\n");
}


int main()
{
    char input[MAX_INPUT];
    Token tokens[MAX_TOKENS];

    printf("========================================\n");
    printf("       ESCAPE SEQUENCE PARSER\n");
    printf("========================================\n");

    printf("\nEnter input:\n> ");

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return 1;
    }

    int count = tokenize(input, tokens);

    if (count == -1)
    {
        return 1;
    }

    display_tokens(tokens, count);

    validate_output(tokens, count);

    return 0;
}
