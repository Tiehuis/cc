/**
 * lexer.h
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"
#include "vec.h"

/* Currently the lex_t stores the current file position that we are reading
 * from. This may be added to in the future */
typedef struct {
    FILE *fd;
} lex_t;

/**
 * Initialize the lexer and return a handle.
 */
lex_t* lex_init(char *filename);

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
