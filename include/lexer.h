#ifndef LEXER_H
#define LEXER_H

#include "token.h"

void lexer_tokenize(const char *input, token_list_t *list);

#endif
