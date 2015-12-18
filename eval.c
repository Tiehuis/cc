/**
 * eval.c
 *
 * Evaluate an ast and return the result.
 */

#include "compiler.h"
#include "eval.h"

eval_t* eval_init(node_t *ast)
{
    eval_t *ctx = xmalloc(sizeof(eval_t));
    ctx->root = ast;
    ctx->curr = ast;
    return ctx;
}

void eval_free(eval_t *ctx)
{
    free(ctx);
}

static long eval_node(node_t *node)
{
    switch (node->id) {
    case TOK_PLUS:
        return eval_node(node->left) + eval_node(node->right);
    case TOK_MINUS:
        return eval_node(node->left) - eval_node(node->right);
    case TOK_MOD:
        return eval_node(node->left) % eval_node(node->right);
    case TOK_MULTIPLY:
        return eval_node(node->left) * eval_node(node->right);
    case TOK_DIV:
        return eval_node(node->left) / eval_node(node->right);
    case TOK_BWAND:
        return eval_node(node->left) & eval_node(node->right);
    case TOK_BWOR:
        return eval_node(node->left) | eval_node(node->right);
    case TOK_BWXOR:
        return eval_node(node->left) ^ eval_node(node->right);
    case TOK_NUMBER:
        return node->value;
    default:
        printf("%s\n", __token_names[node->id]);
        xerror("Invalid token");
    }
}

long eval_compute(eval_t *ctx)
{
    return eval_node(ctx->root);
}
