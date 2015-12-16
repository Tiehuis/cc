#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "vec.h"

VEC_DECLARE(token_t*, token);

int main(void)
{
    lex_t *lctx = lex_init("test.c");
    vec_token_t *tokens = vec_token_init();

    token_t *tok;
    while ((tok = lex_token(lctx)) != NULL) {
        vec_token_push(tokens, tok);
    }

    lex_free(lctx);

    /* Put input into parser */
    rdp_t *rctx = rdp_init(tokens->data, tokens->len);

    /* Get a root node for an AST */

    rdp_free(rctx);

    /*
    for (int i = 0; i < tokens->len; ++i) {
        token_t *t = tokens->data[i];
        printf("%s: ", __token_names[t->type]);
        printf("%s\n", t->is_literal ? t->literal : "");
    }
    */

}

