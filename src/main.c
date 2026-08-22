#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"
#include "executor.h"

static void print_pipeline(const pipeline_t *pipeline)
{
    printf("\n============ PIPELINE ============\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        const command_t *cmd = &pipeline->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("--------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < cmd->argc; j++)
        {
            printf("argv[%d] = %s\n", j, cmd->argv[j]);
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
    printf("=====================================\n");
    printf("            Shellforge\n");
    printf("    A Unix Style Shell written in C\n");
    printf("=====================================\n");

    while (1)
    {
        char *line = readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\n");
            break;
        }

        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        add_history(line);

        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;
        }

        token_list_t tokens;
        pipeline_t pipeline;

        /* Lexing */
        lexer_tokenize(line, &tokens);

        /* Display tokens */
        token_print(&tokens);

        /* Parsing */
        if (parser(&tokens, &pipeline) == 0)
        {
            printf("Parser error\n");
            free(line);
            continue;
        }

        /* Variable expansion */
        expand_variables(&pipeline);

        /* Display pipeline */
        print_pipeline(&pipeline);

        /* Execute commands */
        for (int i = 0; i < pipeline.command_count; i++)
        {
            execute_command(&pipeline.commands[i]);
        }

        free(line);
    }

    return 0;
}
