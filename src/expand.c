#include <stdlib.h>
#include <string.h>

#include "expand.h"

static char *expand_string(const char *text)
{
    if (text == NULL)
        return NULL;

    char *result = malloc(strlen(text) + 1);

    if (result == NULL)
        return NULL;

    strcpy(result, text);

    return result;
}

void expand_variables(pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->command_count; i++)
    {
        command_t *cmd = &pipeline->commands[i];

        for (int j = 0; j < cmd->argc; j++)
        {
            if (cmd->argv[j] == NULL)
                continue;

            if (cmd->argv[j][0] == '$')
            {
                const char *value =
                    getenv(cmd->argv[j] + 1);

                if (value != NULL)
                {
                    char *expanded = expand_string(value);

                    if (expanded != NULL)
                        cmd->argv[j] = expanded;
                }
            }
        }
    }
}
