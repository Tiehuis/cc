#ifndef TOKEN_H
#define TOKEN_H

enum {
#define X(x) x,
#   include "tokens.include"
#undef X
__token_dummy__  /* Dummy element for trailing comma */
};

static const char *__token_names[] = {
#define X(x) #x,
#   include "tokens.include"
#undef X
"__token_dummy__"
};

typedef struct {
    int type;
    int is_literal;
    union {
        int id;
        char *literal;
    };
} token_t;

int token_is_operator(token_t *token);

#endif
