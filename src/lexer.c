#include <ctype.h>
#include "lexer.h"

void lexer_tokenize(const char *input, token_list_t *list) {
    token_list_init(list);

    int i = 0;

    while (input[i] != '\0') {

        /* Skip spaces */
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        /* Pipe */
        if (input[i] == '|') {
            token_add(list, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Input redirection */
        if (input[i] == '<') {
            token_add(list, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        /* Output / append */
        if (input[i] == '>') {
            if (input[i + 1] == '>') {
                token_add(list, TOKEN_APPEND, ">>");
                i += 2;
            } else {
                token_add(list, TOKEN_OUTPUT, ">");
                i++;
            }
            continue;
        }

        /* Background */
        if (input[i] == '&') {
            token_add(list, TOKEN_BACKGROUND, "&");
            i++;
            continue;
        }

        /* Backslash */
        if (input[i] == '\\') {
            char word[2];
            word[0] = '\\';
            word[1] = '\0';

            token_add(list, TOKEN_WORD, word);
            i++;
            continue;
        }

        /* Word / quoted word */
        {
            char word[MAX_TOKEN_LEN];
            int j = 0;

            while (input[i] != '\0' &&
                   !isspace((unsigned char)input[i]) &&
                   input[i] != '|' &&
                   input[i] != '<' &&
                   input[i] != '>' &&
                   input[i] != '&') {

                /* Single quotes */
                if (input[i] == '\'') {
                    i++;

                    while (input[i] != '\0' && input[i] != '\'') {
                        if (j < MAX_TOKEN_LEN - 1)
                            word[j++] = input[i];
                        i++;
                    }

                    if (input[i] == '\'')
                        i++;

                    continue;
                }

                /* Double quotes */
                if (input[i] == '"') {
                    i++;

                    while (input[i] != '\0' && input[i] != '"') {
                        if (input[i] == '\\' &&
                            input[i + 1] != '\0') {
                            i++;
                        }

                        if (j < MAX_TOKEN_LEN - 1)
                            word[j++] = input[i];

                        i++;
                    }

                    if (input[i] == '"')
                        i++;

                    continue;
                }

                /* Normal character */
                if (j < MAX_TOKEN_LEN - 1)
                    word[j++] = input[i];

                i++;
            }

            word[j] = '\0';

            if (j > 0)
                token_add(list, TOKEN_WORD, word);
        }
    }

    token_add(list, TOKEN_END, "END");
}
