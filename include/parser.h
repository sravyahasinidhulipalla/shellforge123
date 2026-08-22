#ifndef PARSER_H
#define PARSER_H

#include "token.h"

#define MAX_ARGS 100
#define MAX_COMMANDS 100

typedef struct {
    char *argv[MAX_ARGS];
    int argc;

    char *input_file;
    char *output_file;

    int append;
    int background;
} command_t;

typedef struct {
    command_t commands[MAX_COMMANDS];
    int command_count;
} pipeline_t;

int parser(const token_list_t *tokens, pipeline_t *pipeline);

#endif
