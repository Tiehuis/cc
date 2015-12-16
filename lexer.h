#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "vec.h"

enum {
#define X(x) x,
#include "tokens.include"
#undef X
__lex_dummy__  /* Dummy element for trailing comma */
};

static const char *__token_names[] = {
#define X(x) #x,
#include "tokens.include"
#undef X
"__lex_dummy__"
};

typedef struct {
    int type;
    int is_literal;
    union {
        int id;
        char *literal;
    };
} token_t;

typedef struct {
    FILE *fd;
} lex_t;

lex_t* lex_init(char *filename);

void lex_free(lex_t *ctx);

token_t* lex_token(lex_t *ctx);

#endif
