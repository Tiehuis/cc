/**
 * parser.c
 *
 * Parse things.
 *
 * This is a simple recursive descent parser. We keep a context 'ctx' which
 * keeps the current state of the lexeme stream that we are reading from.
 * All lexemes are non-null, with the final lexeme having a type of 'TOK_EOF'.
 */

#include <string.h>
#include <stdlib.h>
#include "token.h"
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

token_t* rdp_peek(rdp_t *ctx)
{
    return ctx->tokens[ctx->position];
}

void rdp_consume(rdp_t *ctx)
{
    ctx->position++;
}

token_t* rdp_pop(rdp_t *ctx)
{
    token_t *t = rdp_peek(ctx);
    rdp_consume(ctx);
    return t;
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
    n->is_term = 0;
    return n;
}

node_t* ast_unary_operator(int id, node_t *operand)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->id = id;
    n->operand = operand;
    n->is_term = 0;
    return n;
}

node_t *ast_number(long value)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->id = LITERAL;
    n->value = value;
    n->is_term = 1;
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
 * <expression> ::= <number> [ <operator> <expression> ]
 */
node_t* rdp_expression(rdp_t *ctx)
{
    rdp_consume_blanks(ctx);

    /* Expect number */
    node_t *left = rdp_number(ctx);
    rdp_consume_blanks(ctx);

    token_t *op = rdp_peek(ctx);

    if (token_is_operator(op)) {
        rdp_consume(ctx);
        rdp_consume_blanks(ctx);
        node_t *right = rdp_expression(ctx);
        return ast_binary_operator(op->type, left, right);
    }
    else {
        return left;
    }
}

node_t* rdp_generate_ast(rdp_t *ctx)
{
    return rdp_expression(ctx);
}
