/**
 * token.h
 *
 * Define all token types.
 */

#ifndef TOKEN_H
#define TOKEN_H

/* The following enum contains all token names with the names 'TOK_*' */
enum {
#define X(x) x,
#   include "token.include"
#undef X
__token_dummy__  /* Dummy element for trailing comma */
};

/* These names can be indexed via the above enum and make debugging slightly
 * easier */
static const char *__token_names[] = {
#define X(x) #x,
#   include "token.include"
#undef X
"__token_dummy__"
};

/* The token type. These tokens are generated within the lexer, but knowledge
 * of this type is also required in the parser.
 *
 * This may be moved into the lexer since it makes more sense there. */
typedef struct {
    int type;
    int is_literal;
    union {
        int id;
        char *literal;
    };
} token_t;

#endif
