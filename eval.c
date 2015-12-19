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
    switch (node->arity) {

    case AST_NULLARY:
        return node->value;

    case AST_UNARY:
        switch (node->id) {
        case TOK_PLUS:
            return +eval_node(node->operand);
        case TOK_MINUS:
            return -eval_node(node->operand);
        case TOK_BWNEG:
            return ~eval_node(node->operand);
        default:
            printf("%s\n", __token_names[node->id]);
            xerror("Invalid token");
        }

    case AST_BINARY:
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
        case TOK_LOR:
            return eval_node(node->left) || eval_node(node->right);
        case TOK_LAND:
            return eval_node(node->left) && eval_node(node->right);
        case TOK_LSHIFT:
            return eval_node(node->left) << eval_node(node->right);
        case TOK_RSHIFT:
            return eval_node(node->left) >> eval_node(node->right);
        case TOK_LT:
            return eval_node(node->left) < eval_node(node->right);
        case TOK_GT:
            return eval_node(node->left) > eval_node(node->right);
        case TOK_LTE:
            return eval_node(node->left) <= eval_node(node->right);
        case TOK_GTE:
            return eval_node(node->left) >= eval_node(node->right);
        case TOK_EQUALITY:
            return eval_node(node->left) == eval_node(node->right);
        case TOK_NEQUALITY:
            return eval_node(node->left) != eval_node(node->right);
        default:
            printf("%s\n", __token_names[node->id]);
            xerror("Invalid token");
        }

    default:
        printf("%s\n", __token_names[node->id]);
        xerror("Invalid token");
    }

}

long eval_compute(eval_t *ctx)
{
    return eval_node(ctx->root);
}
