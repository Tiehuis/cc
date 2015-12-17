#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"
#include "vec.h"

typedef struct {
    FILE *fd;
} lex_t;

lex_t* lex_init(char *filename);

void lex_free(lex_t *ctx);

token_t* lex_token(lex_t *ctx);

#endif
