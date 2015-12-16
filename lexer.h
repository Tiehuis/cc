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

char *token_names[] = {
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

/* Initialize vec_token_t */
VEC_DECLARE(token_t*, token);

/* Initialize vec_char_t/string */
VEC_DECLARE(char, char);

typedef struct {
    FILE *fd;
} lex_t;

#endif
