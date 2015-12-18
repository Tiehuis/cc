/**
 * lexer.h
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"
#include "vec.h"

/**
 * Enum for the type of 'lex_t' input.
 */
enum {
    STRING_BACKED,
    FILE_BACKED
};

/* Store either a file descriptor or a string and index. At some point, it may
 * be good to read the file in chunks and use the string based method for most
 * of the character gets. */
typedef struct {
    int type;
    union {
        FILE *fd;
        struct {
            char *string;
            size_t position;
        };
    };
} lex_t;

/**
 * Initialize the lexer and return a handle.
 */
lex_t* lex_init(char *filename, int type);

/**
 * Free any resources associated with the given lexer context.
 */
void lex_free(lex_t *ctx);

/**
 * Lex the next token from the given context. When there exist no more tokens
 * to lex, the token 'TOK_EOF' is returned.
 *
 * Do not expect NULL!
 */
token_t* lex_token(lex_t *ctx);

#endif
