#ifndef EXPAND_H
#define EXPAND_H

#define MAX_EXPANDED_ARGS 100

char **expand_arguments(char **args, int argc);
void free_expanded_arguments(char **args, int count);

#endif
