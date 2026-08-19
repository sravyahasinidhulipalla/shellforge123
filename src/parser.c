#include <stdio.h>
#include "parser.h"

void parser_init(command_list_t *list)
{
    list->count = 0;

    for (int i = 0; i < MAX_TOKENS; i++)
    {
        list->commands[i].argc = 0;
        list->commands[i].input_file = NULL;
        list->commands[i].output_file = NULL;
        list->commands[i].append = 0;
        list->commands[i].background = 0;

        for (int j = 0; j < MAX_ARGS; j++)
            list->commands[i].args[j] = NULL;
    }
}

int parse_tokens(const token_list_t *tokens, command_list_t *list)
{
    parser_init(list);

    if (tokens == NULL || tokens->count == 0)
        return 0;

    command_t *cmd = &list->commands[0];
    list->count = 1;

    for (int i = 0; i < tokens->count; i++)
    {
        token_t *t = &tokens->tokens[i];

        switch (t->type)
        {
            case TOKEN_WORD:
                if (cmd->argc < MAX_ARGS - 1)
                    cmd->args[cmd->argc++] = t->text;
                break;

            case TOKEN_INPUT:
                if (i + 1 < tokens->count &&
                    tokens->tokens[i + 1].type == TOKEN_WORD)
                {
                    cmd->input_file = tokens->tokens[++i].text;
                }
                break;

            case TOKEN_OUTPUT:
            case TOKEN_APPEND:
                if (i + 1 < tokens->count &&
                    tokens->tokens[i + 1].type == TOKEN_WORD)
                {
                    cmd->output_file = tokens->tokens[++i].text;
                    cmd->append = (t->type == TOKEN_APPEND);
                }
                break;

            case TOKEN_BACKGROUND:
                cmd->background = 1;
                break;

            case TOKEN_PIPE:
                if (list->count < MAX_TOKENS)
                {
                    cmd->args[cmd->argc] = NULL;

                    cmd = &list->commands[list->count++];

                    cmd->argc = 0;
                    cmd->input_file = NULL;
                    cmd->output_file = NULL;
                    cmd->append = 0;
                    cmd->background = 0;
                }
                break;

            case TOKEN_END:
                i = tokens->count;
                break;

            default:
                break;
        }
    }

    for (int i = 0; i < list->count; i++)
        list->commands[i].args[list->commands[i].argc] = NULL;

    return list->count;
}
