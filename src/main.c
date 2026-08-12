#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "lexer.h"
#include "token.h"

#define MAX_HISTORY 100

int main(void) {
    char *input;
    char history[MAX_HISTORY][1024];
    int history_count = 0;

    printf("Welcome to milestone 2\n\n");

    while (1) {
        input = readline("shellforge$ ");

        if (input == NULL)
            break;

        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            free(input);
            printf("Exiting..\n");
            break;
        }

        add_history(input);

        if (history_count < MAX_HISTORY) {
            strncpy(history[history_count], input, 1023);
            history[history_count][1023] = '\0';
            history_count++;
        }

        if (strcmp(input, "history") == 0) {
            printf("\n------ Command History ------\n");

            for (int i = 0; i < history_count - 1; i++)
                printf("%d : %s\n", i + 1, history[i]);

            printf("-----------------------------\n\n");

            free(input);
            continue;
        }

        token_list_t tokens;
        lexer_tokenize(input, &tokens);
        token_print(&tokens);

        free(input);
    }

    return 0;
}
