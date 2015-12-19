/**
 * parser.c
 *
 * Parse things.
 *
 * This is a simple recursive descent parser. We keep a context 'ctx' which
 * keeps the current state of the lexeme stream that we are reading from.
 * All lexemes are non-null, with the final lexeme having a type of 'TOK_EOF'.
 *
 * The current grammar is a simplified version of
 *    'www.cs.man.ac.uk/~pjj/bnf/c_syntax.bnf'
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "token.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

/**
 * rdp context functions.
 */

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

void rdp_fail(rdp_t *ctx)
{
    longjmp(ctx->env, 1);
}

/**
 * Node generation functions.
 */

static node_t* ast_binary_operator(int id, node_t *left, node_t *right)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->arity = AST_BINARY;
    n->id = id;
    n->left = left;
    n->right = right;
    return n;
}

static node_t* ast_unary_operator(int id, node_t *operand)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->arity = AST_UNARY;
    n->id = id;
    n->operand = operand;
    return n;
}

static node_t *ast_number(long value)
{
    node_t *n = xmalloc(sizeof(node_t));
    n->arity = AST_NULLARY;
    n->id = TOK_NUMBER;
    n->value = value;
    return n;
}

/**
 * Recursive Descent Parser functions.
 */

static node_t* rdp_expression(rdp_t *ctx);

/**
 * <const> : [0-9]
 */
static node_t* rdp_const(rdp_t *ctx)
{
    rdp_consume_blanks(ctx);
    token_t *t = rdp_pop(ctx);

    if (t->type != TOK_NUMBER) {
        rdp_fail(ctx);
        return NULL; /* Unreached */
    }
    else {
        /* Parse number into value */
        long value = strtol(t->literal, NULL, 10);
        return ast_number(value);
    }
}

/**
 * <primary_exp> : <const>
 *               | '(' <expression> ')'
 */
static node_t* rdp_primary_exp(rdp_t *ctx)
{
    rdp_consume_blanks(ctx);
    token_t *left = rdp_peek(ctx);

    if (left->type == TOK_LPAREN) {
        rdp_consume(ctx);
        node_t *expr = rdp_expression(ctx);

        rdp_consume_blanks(ctx);
        token_t *right = rdp_pop(ctx);

        if (right->type == TOK_RPAREN) {
            return expr;
        }
        else {
            rdp_fail(ctx);
            return NULL; /* Unreached */
        }
    }
    else {
        return rdp_const(ctx);
    }
}

/**
 * <unary_exp> : <primary_exp>
 */
static node_t* rdp_unary_exp(rdp_t *ctx)
{
    rdp_consume_blanks(ctx);
    token_t *op = rdp_peek(ctx);

    switch (op->type) {
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_BWNEG:
            rdp_consume(ctx);
            return ast_unary_operator(op->type, rdp_unary_exp(ctx));
        default:
            return rdp_primary_exp(ctx);
    }

    return rdp_primary_exp(ctx);
}

/**
 * <mult_exp> : <unary_exp>
 *            | <mult_exp> '*' <unary_exp>
 *            | <mult_exp> '/' <unary_exp>
 *            | <mult_exp> '%' <unary_exp>
 */
static node_t* rdp_mult_exp(rdp_t *ctx)
{
    node_t *left = rdp_unary_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_MULTIPLY:
            case TOK_DIV:
            case TOK_MOD:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_unary_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <additive_exp> : <mult_exp>
 *                | <additive_exp> '+' <mult_exp>
 *                | <additive_exp> '-' <mult_exp>
 */
static node_t* rdp_additive_exp(rdp_t *ctx)
{
    node_t *left = rdp_mult_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_PLUS:
            case TOK_MINUS:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_mult_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <shift_exp> : <additive_exp>
 *             | <shift_exp> '<<' <additive_exp>
 *             | <shift_exp> '>>' <additive_exp>
 */
static node_t* rdp_shift_exp(rdp_t *ctx)
{
    node_t *left = rdp_additive_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_LSHIFT:
            case TOK_RSHIFT:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_additive_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <relational_exp> : <shift_exp>
 *                  | <relational_exp> '<' <shift_exp>
 *                  | <relational_exp> '>' <shift_exp>
 */
static node_t* rdp_relational_exp(rdp_t *ctx)
{
    node_t *left = rdp_shift_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_LT:
            case TOK_LTE:
            case TOK_GT:
            case TOK_GTE:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_shift_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <equality_exp> : <relational_exp>
 *                | <equality_exp> '==' <relational_exp>
 *                | <equality_exp> '!=' <relational_exp>
 */
static node_t* rdp_equality_exp(rdp_t *ctx)
{
    node_t *left = rdp_relational_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_EQUALITY:
            case TOK_NEQUALITY:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_relational_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <and_exp> : <relational_exp>
 *           | <and_exp> '&' <relational_exp>
 */
static node_t* rdp_and_exp(rdp_t *ctx)
{
    node_t *left = rdp_equality_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_BWAND:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_equality_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <xor_exp> : <and_exp>
 *           | <xor_exp> '^' <and_exp>
 */
static node_t* rdp_xor_exp(rdp_t *ctx)
{
    node_t *left = rdp_and_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_BWXOR:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_and_exp(ctx));
                break;
            default:
                return left;
        }
    }
}


/**
 * <ior_exp> : <xor_exp>
 *           | <ior_exp> '|' <xor_exp>
 */
static node_t* rdp_ior_exp(rdp_t *ctx)
{
    node_t *left = rdp_xor_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_BWOR:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_xor_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <land_exp> : <ior_exp>
 *            | <land_exp> '&&' <ior_exp>
 */
static node_t* rdp_land_exp(rdp_t *ctx)
{
    node_t *left = rdp_ior_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_LAND:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_ior_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <lor_exp> : <land_exp>
 *           | <lor_exp> '||' <land_exp>
 */
static node_t* rdp_lor_exp(rdp_t *ctx)
{
    node_t *left = rdp_land_exp(ctx);

    while (1) {
        rdp_consume_blanks(ctx);
        token_t *op = rdp_peek(ctx);

        switch (op->type) {
            case TOK_LOR:
                rdp_consume(ctx);
                left = ast_binary_operator(op->type, left, rdp_land_exp(ctx));
                break;
            default:
                return left;
        }
    }
}

/**
 * <expression> : <lor_exp>
 */
static node_t* rdp_expression(rdp_t *ctx)
{
    return rdp_lor_exp(ctx);
}

/**
 * Generate an AST from a sequence of lexemes.
 */
node_t* rdp_generate_ast(rdp_t *ctx)
{
    if (!setjmp(ctx->env)) {
        return rdp_expression(ctx);
    }
    else {
        return NULL;
    }
}
