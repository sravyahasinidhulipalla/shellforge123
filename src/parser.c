#include <string.h>
#include "parser.h"

int parser(const token_list_t *tokens, pipeline_t *pipeline)
{
    if (tokens == NULL || pipeline == NULL)
        return 0;

    pipeline->command_count = 0;

    int cmd_index = 0;

    pipeline->commands[0].argc = 0;
    pipeline->commands[0].input_file = NULL;
    pipeline->commands[0].output_file = NULL;
    pipeline->commands[0].append = 0;
    pipeline->commands[0].background = 0;

    for (int i = 0; i < MAX_ARGS; i++)
        pipeline->commands[0].argv[i] = NULL;

    for (int i = 0; i < tokens->count; i++)
    {
        token_t *token = &tokens->tokens[i];

        if (token->type == TOKEN_WORD)
        {
            command_t *cmd = &pipeline->commands[cmd_index];

            if (cmd->argc < MAX_ARGS - 1)
            {
                cmd->argv[cmd->argc++] = token->text;
            }
        }
        else if (token->type == TOKEN_PIPE)
        {
            pipeline->commands[cmd_index].argv[
                pipeline->commands[cmd_index].argc
            ] = NULL;

            cmd_index++;

            if (cmd_index >= MAX_COMMANDS)
                break;

            pipeline->commands[cmd_index].argc = 0;
            pipeline->commands[cmd_index].input_file = NULL;
            pipeline->commands[cmd_index].output_file = NULL;
            pipeline->commands[cmd_index].append = 0;
            pipeline->commands[cmd_index].background = 0;

            for (int j = 0; j < MAX_ARGS; j++)
                pipeline->commands[cmd_index].argv[j] = NULL;
        }
        else if (token->type == TOKEN_INPUT)
        {
            if (i + 1 < tokens->count &&
                tokens->tokens[i + 1].type == TOKEN_WORD)
            {
                pipeline->commands[cmd_index].input_file =
                    tokens->tokens[++i].text;
            }
        }
        else if (token->type == TOKEN_OUTPUT)
        {
            if (i + 1 < tokens->count &&
                tokens->tokens[i + 1].type == TOKEN_WORD)
            {
                pipeline->commands[cmd_index].output_file =
                    tokens->tokens[++i].text;

                pipeline->commands[cmd_index].append = 0;
            }
        }
        else if (token->type == TOKEN_APPEND)
        {
            if (i + 1 < tokens->count &&
                tokens->tokens[i + 1].type == TOKEN_WORD)
            {
                pipeline->commands[cmd_index].output_file =
                    tokens->tokens[++i].text;

                pipeline->commands[cmd_index].append = 1;
            }
        }
        else if (token->type == TOKEN_BACKGROUND)
        {
            pipeline->commands[cmd_index].background = 1;
        }
        else if (token->type == TOKEN_END)
        {
            break;
        }
    }

    for (int i = 0; i <= cmd_index; i++)
    {
        pipeline->commands[i].argv[
            pipeline->commands[i].argc
        ] = NULL;
    }

    pipeline->command_count = cmd_index + 1;

    return 1;
}
