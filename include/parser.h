#ifndef PARSER_H
#define PARSER_H

#include "token.h"

#define MAX_ARGS 100

typedef struct {
    char *args[MAX_ARGS];
    int argc;
    char *input_file;
    char *output_file;
    int append;
    int background;
} command_t;

typedef struct {
    command_t commands[MAX_TOKENS];
    int count;
} command_list_t;

void parser_init(command_list_t *list);
int parse_tokens(const token_list_t *tokens, command_list_t *list);

#endif
