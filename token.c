#include "token.h"

int token_is_operator(token_t *token)
{
    switch (token->type) {
    case TOK_PLUS:
    case TOK_MINUS:
    case TOK_MULTIPLY:
    case TOK_DIV:
    case TOK_MOD:
        return 1;
    default:
        return 0;
    }
}
