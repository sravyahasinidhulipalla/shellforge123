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

/* =========================================================
   DISPLAY PIPELINE
   ========================================================= */

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

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    /* Setup SIGCHLD handler for background processes */
    setup_background_handler();

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

        /* -------------------------------------------------
           LEXING
           ------------------------------------------------- */

        lexer_tokenize(line, &tokens);

        /* Display tokens */
        token_print(&tokens);

        /* -------------------------------------------------
           PARSING
           ------------------------------------------------- */

        if (parser(&tokens, &pipeline) == 0)
        {
            printf("Parser error\n");
            free(line);
            continue;
        }

        /* -------------------------------------------------
           VARIABLE EXPANSION
           ------------------------------------------------- */

        expand_variables(&pipeline);

        /* -------------------------------------------------
           DISPLAY PIPELINE
           ------------------------------------------------- */

        print_pipeline(&pipeline);

        /* -------------------------------------------------
           EXECUTE COMPLETE PIPELINE
           ------------------------------------------------- */

        execute_pipeline(&pipeline);

        free(line);
    }

    return 0;
}
