#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include "expand.h"

char **expand_arguments(char **args, int argc)
{
    char **result = malloc(sizeof(char *) * MAX_EXPANDED_ARGS);
    int count = 0;

    if (result == NULL)
        return NULL;

    for (int i = 0; i < argc && count < MAX_EXPANDED_ARGS - 1; i++)
    {
        glob_t g;

        if (glob(args[i], 0, NULL, &g) == 0 && g.gl_pathc > 0)
        {
            for (size_t j = 0;
                 j < g.gl_pathc && count < MAX_EXPANDED_ARGS - 1;
                 j++)
            {
                result[count] = malloc(strlen(g.gl_pathv[j]) + 1);

                if (result[count] != NULL)
                {
                    strcpy(result[count], g.gl_pathv[j]);
                    count++;
                }
            }

            globfree(&g);
        }
        else
        {
            result[count] = malloc(strlen(args[i]) + 1);

            if (result[count] != NULL)
            {
                strcpy(result[count], args[i]);
                count++;
            }

            globfree(&g);
        }
    }

    result[count] = NULL;
    return result;
}

void free_expanded_arguments(char **args, int count)
{
    if (args == NULL)
        return;

    for (int i = 0; i < count; i++)
        free(args[i]);

    free(args);
}
