#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "token.h"
#include "parser.h"

static void print_pipeline(const command_list_t *list)
{
    printf("\n============ PIPELINE ============\n");

    for (int i = 0; i < list->count; i++)
    {
        const command_t *cmd = &list->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("--------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < cmd->argc; j++)
        {
            printf("argv[%d] = %s\n", j, cmd->args[j]);
        }

        printf("Input       : %s\n",
               cmd->input_file ? cmd->input_file : "None");

        printf("Output      : %s\n",
               cmd->output_file ? cmd->output_file : "None");

        printf("Append      : %s\n",
               cmd->append ? "Yes" : "No");

        printf("Background  : %s\n",
               cmd->background ? "Yes" : "No");

        printf("==================================\n");
    }
}

int main(void)
{
    char input[1024];

    printf("========================================\n");
    printf("            Shellforge\n");
    printf("     A Unix Style Shell written in C\n");
    printf("========================================\n");

    while (1)
    {
        printf("shellforge$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        if (strcmp(input, "exit\n") == 0)
            break;

        token_list_t tokens;
        command_list_t commands;

        token_list_init(&tokens);

        lexer_tokenize(input, &tokens);

        token_print(&tokens);

        parse_tokens(&tokens, &commands);

        print_pipeline(&commands);
    }

    return 0;
}
