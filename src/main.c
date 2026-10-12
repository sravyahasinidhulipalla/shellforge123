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
#include "jobs.h"

static void print_pipeline(const pipeline_t *pipeline)
{
    printf("\n========== PARSED PIPELINE ==========\n");

    for (int i = 0;
         i < pipeline->command_count;
         i++)
    {
        const command_t *cmd =
            &pipeline->commands[i];

        printf("Command %d:\n", i + 1);

        printf("  argc = %d\n",
               cmd->argc);

        for (int j = 0;
             j < cmd->argc;
             j++)
        {
            printf("  argv[%d] = %s\n",
                   j,
                   cmd->argv[j]);
        }

        if (cmd->input_file != NULL)
        {
            printf("  input = %s\n",
                   cmd->input_file);
        }

        if (cmd->output_file != NULL)
        {
            printf("  output = %s\n",
                   cmd->output_file);
        }

        printf("  append = %d\n",
               cmd->append);

        printf("  background = %d\n",
               cmd->background);
    }

    printf("=====================================\n\n");
}

int main(void)
{
    jobs_init();

    setup_job_control();
    setup_background_handler();

    printf("=====================================\n");
    printf("      Shellforge\n");
    printf(" A Unix Style Shell written in C\n");
    printf("=====================================\n");

    while (1)
    {
        char *line =
            readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\nExiting...\n");
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
            printf("Exiting...\n");
            free(line);
            break;
        }

        token_list_t tokens;
        pipeline_t pipeline;

        lexer_tokenize(line, &tokens);

        token_print(&tokens);

        if (parser(&tokens, &pipeline) == 0)
        {
            printf("Parser error\n");

            free(line);
            continue;
        }

        expand_variables(&pipeline);

        print_pipeline(&pipeline);

        if (pipeline.command_count == 1 &&
            is_builtin(&pipeline.commands[0]))
        {
            int result =
                execute_builtin(
                    &pipeline.commands[0]);

            if (result == 1)
            {
                printf("Exiting...\n");
                free(line);
                break;
            }

            free(line);
            continue;
        }

        execute_pipeline(&pipeline, line);

        free(line);
    }

    return 0;
}
