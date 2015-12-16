/**
 * parser.c
 *
 * Parse things.
 */

#include <string.h>
#include <stdlib.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

rdp_t* rdp_init(token_t **tokens, size_t length)
{
    rdp_t *r = xmalloc(sizeof(rdp_t));
    r->position = 0;
    r->tokens = tokens;
    r->length = length;
    return r;
}

void rdp_free(rdp_t *ctx)
{
    free(ctx);
}

token_t* rdp_pop(rdp_t *ctx)
{
    return ctx->tokens[ctx->position++];
}

token_t* rdp_peek(rdp_t *ctx)
{
    return ctx->tokens[ctx->position];
}

void rdp_consume(rdp_t *ctx)
{
    ctx->position++;
}

void rdp_consume_blanks(rdp_t *ctx)
{
    while (ctx->tokens[ctx->position]->type == TOK_SPACE)
        ctx->position++;
}

node_t* ast_binary_operator(int id, node_t *left, node_t *right)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->id = id;
    n->left = left;
    n->right = right;
    return n;
}

node_t* ast_unary_operator(int id, node_t *operand)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->id = id;
    n->operand = operand;
    return n;
}

node_t *ast_number(long value)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->id = LITERAL;
    n->value = value;
    return n;
}

/**
 * <operator> ::= "+"
 */
node_t* rdp_operator(rdp_t *ctx)
{
    return NULL;
}

/**
 * <number> ::= [0-9]
 */
node_t* rdp_number(rdp_t *ctx)
{
    /* Read the current token */
    token_t *t = rdp_pop(ctx);

    if (t->type != TOK_NUMBER) {
        return NULL;
    }
    else {
        /* Parse number into value */
        long value = strtol(t->literal, NULL, 10);
        return ast_number(value);
    }
}

/**
 * <expression> ::= <number> [ <operator> <number> ]
 * <
 */
node_t* rdp_expression(rdp_t *ctx)
{
    /* Expect number */
    node_t *left = rdp_number(ctx);

    if (rdp_peek(ctx)->type == TOK_PLUS) {
        node_t *right = rdp_number(ctx);
        return ast_binary_operator(TOK_PLUS, left, right);
    }

    return left;
}
