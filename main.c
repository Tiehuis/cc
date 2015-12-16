#include <stdio.h>
#include "lexer.h"
#include "vec.h"

VEC_DECLARE(token_t*, token);

int main(void)
{
    lex_t *ctx = lex_init("test.c");
    vec_token_t *tokens = vec_token_init();

    token_t *tok;
    while ((tok = lex_token(ctx)) != NULL) {
        vec_token_push(tokens, tok);
    }

    for (int i = 0; i < tokens->len; ++i) {
        token_t *t = tokens->data[i];
        printf("%s: ", __token_names[t->type]);
        printf("%s\n", t->is_literal ? t->literal : "");
    }

    lex_free(ctx);
}

