#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_TOKENS 100

/* Token types */
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_APPEND,
    TOKEN_END
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;
    char value[100];
} Token;

/* Execution command structure */
typedef struct Command {
    char *args[MAX_TOKENS];
    int argc;
    char *input_file;
    char *output_file;
    int append;
    struct Command *next;
} Command;

/* Convert token type to string */
const char *tokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_WORD:
            return "WORD";
        case TOKEN_PIPE:
            return "PIPE";
        case TOKEN_INPUT:
            return "INPUT";
        case TOKEN_OUTPUT:
            return "OUTPUT";
        case TOKEN_APPEND:
            return "APPEND";
        case TOKEN_END:
            return "END";
        default:
            return "UNKNOWN";
    }
}

/* Add a token */
void addToken(Token tokens[], int *count, TokenType type, const char *value) {
    if (*count >= MAX_TOKENS - 1) {
        printf("Error: Too many tokens.\n");
        return;
    }

    tokens[*count].type = type;
    strcpy(tokens[*count].value, value);
    (*count)++;
}

/* Lexical analysis */
int tokenize(char *input, Token tokens[]) {
    int count = 0;
    int i = 0;

    while (input[i] != '\0' && input[i] != '\n') {

        /* Handle whitespace */
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        /* Pipe delimiter */
        if (input[i] == '|') {
            addToken(tokens, &count, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Input redirection */
        if (input[i] == '<') {
            addToken(tokens, &count, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        /* Output redirection */
        if (input[i] == '>') {

            /* Append redirection >> */
            if (input[i + 1] == '>') {
                addToken(tokens, &count, TOKEN_APPEND, ">>");
                i += 2;
            } else {
                addToken(tokens, &count, TOKEN_OUTPUT, ">");
                i++;
            }

            continue;
        }

        /* Read a normal word */
        if (input[i] != '\0') {
            char word[100];
            int j = 0;

            while (input[i] != '\0' &&
                   !isspace((unsigned char)input[i]) &&
                   input[i] != '|' &&
                   input[i] != '<' &&
                   input[i] != '>') {

                if (j < 99) {
                    word[j++] = input[i];
                }

                i++;
            }

            word[j] = '\0';

            if (j > 0) {
                addToken(tokens, &count, TOKEN_WORD, word);
            }
        }
    }

    addToken(tokens, &count, TOKEN_END, "END");

    return count;
}

/* Display token stream */
void printTokens(Token tokens[], int count) {
    printf("\n========== TOKEN STREAM ==========\n");

    for (int i = 0; i < count; i++) {
        printf("Token %d: %-8s -> %s\n",
               i + 1,
               tokenTypeName(tokens[i].type),
               tokens[i].value);
    }

    printf("==================================\n");
}

/* Validate token stream */
int validateTokens(Token tokens[], int count) {

    if (count <= 1) {
        printf("Empty command.\n");
        return 0;
    }

    /* Command cannot start with pipe */
    if (tokens[0].type == TOKEN_PIPE) {
        printf("Syntax Error: Command cannot start with '|'.\n");
        return 0;
    }

    for (int i = 0; i < count - 1; i++) {

        /* Pipe cannot be followed by pipe */
        if (tokens[i].type == TOKEN_PIPE &&
            tokens[i + 1].type == TOKEN_PIPE) {

            printf("Syntax Error: Consecutive pipes are not allowed.\n");
            return 0;
        }

        /* Pipe cannot be followed by redirection */
        if (tokens[i].type == TOKEN_PIPE &&
            (tokens[i + 1].type == TOKEN_INPUT ||
             tokens[i + 1].type == TOKEN_OUTPUT ||
             tokens[i + 1].type == TOKEN_APPEND)) {

            printf("Syntax Error: Invalid command after pipe.\n");
            return 0;
        }

        /* Redirection must be followed by a filename */
        if ((tokens[i].type == TOKEN_INPUT ||
             tokens[i].type == TOKEN_OUTPUT ||
             tokens[i].type == TOKEN_APPEND)) {

            if (tokens[i + 1].type != TOKEN_WORD) {
                printf("Syntax Error: Redirection requires a filename.\n");
                return 0;
            }
        }

        /* Pipe must have a command before it */
        if (tokens[i].type == TOKEN_PIPE &&
            i == 0) {

            printf("Syntax Error: Pipe cannot appear at beginning.\n");
            return 0;
        }
    }

    /* Command cannot end with pipe */
    if (tokens[count - 2].type == TOKEN_PIPE) {
        printf("Syntax Error: Command cannot end with '|'.\n");
        return 0;
    }

    printf("Token stream is valid.\n");
    return 1;
}

/* Create command structure */
Command *createCommand() {
    Command *cmd = malloc(sizeof(Command));

    if (cmd == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;
    cmd->next = NULL;

    return cmd;
}

/* Parse tokens into execution structures */
Command *parseTokens(Token tokens[], int count) {

    Command *head = createCommand();
    Command *current = head;

    for (int i = 0; i < count - 1; i++) {

        if (tokens[i].type == TOKEN_WORD) {

            current->args[current->argc] =
                strdup(tokens[i].value);

            current->argc++;
        }

        else if (tokens[i].type == TOKEN_INPUT) {

            if (i + 1 < count &&
                tokens[i + 1].type == TOKEN_WORD) {

                current->input_file =
                    strdup(tokens[++i].value);
            }
        }

        else if (tokens[i].type == TOKEN_OUTPUT) {

            if (i + 1 < count &&
                tokens[i + 1].type == TOKEN_WORD) {

                current->output_file =
                    strdup(tokens[++i].value);

                current->append = 0;
            }
        }

        else if (tokens[i].type == TOKEN_APPEND) {

            if (i + 1 < count &&
                tokens[i + 1].type == TOKEN_WORD) {

                current->output_file =
                    strdup(tokens[++i].value);

                current->append = 1;
            }
        }

        else if (tokens[i].type == TOKEN_PIPE) {

            current->args[current->argc] = NULL;

            current->next = createCommand();
            current = current->next;
        }
    }

    current->args[current->argc] = NULL;

    return head;
}

/* Display parse tree / execution structure */
void printParseTree(Command *head) {

    printf("\n======= PARSE / EXECUTION TREE =======\n");

    Command *current = head;
    int commandNumber = 1;

    while (current != NULL) {

        printf("\nCommand %d\n", commandNumber);

        printf("  Arguments: ");

        for (int i = 0; i < current->argc; i++) {
            printf("[%s] ", current->args[i]);
        }

        printf("\n");

        if (current->input_file != NULL) {
            printf("  Input    : %s\n",
                   current->input_file);
        }

        if (current->output_file != NULL) {
            printf("  Output   : %s",
                   current->output_file);

            if (current->append)
                printf(" (append)");

            printf("\n");
        }

        if (current->next != NULL) {
            printf("  |\n");
            printf("  V\n");
        }

        current = current->next;
        commandNumber++;
    }

    printf("\n=======================================\n");
}

/* Free memory */
void freeCommands(Command *head) {

    Command *current = head;

    while (current != NULL) {

        Command *next = current->next;

        for (int i = 0; i < current->argc; i++) {
            free(current->args[i]);
        }

        if (current->input_file)
            free(current->input_file);

        if (current->output_file)
            free(current->output_file);

        free(current);

        current = next;
    }
}

/* Main function */
int main() {

    char input[MAX_INPUT];
    Token tokens[MAX_TOKENS];

    printf("============================================\n");
    printf("     LINUX COMMAND LEXER AND PARSER\n");
    printf("============================================\n");

    printf("\nEnter a Linux command:\n");
    printf("> ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    /* Handle empty command */
    int onlyWhitespace = 1;

    for (int i = 0; input[i] != '\0'; i++) {
        if (!isspace((unsigned char)input[i])) {
            onlyWhitespace = 0;
            break;
        }
    }

    if (onlyWhitespace) {
        printf("\nEmpty command detected.\n");
        return 0;
    }

    /* Step 1: Tokenization */
    int tokenCount = tokenize(input, tokens);

    /* Step 2: Debug token output */
    printTokens(tokens, tokenCount);

    /* Step 3: Validate token stream */
    if (!validateTokens(tokens, tokenCount)) {
        return 1;
    }

    /* Step 4: Parser */
    Command *commands =
        parseTokens(tokens, tokenCount);

    /* Step 5: Display execution structure */
    printParseTree(commands);

    /* Free memory */
    freeCommands(commands);

    return 0;
}
