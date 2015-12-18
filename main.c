#include <stdio.h>
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "vec.h"
#include "eval.h"

VEC_DECLARE(token_t*, token);

void print_tree(node_t *node, size_t depth)
{
    switch (node->id) {
    case LITERAL:
        printf("(%zd): %ld\n", depth, node->value);
        break;
    default:
        printf("(%zd): %s\n", depth, __token_names[node->id]);
        break;
    }

    /* Need to change when adding unary operators */
    if (!node->is_term) {
        if (node->left)
            print_tree(node->left, depth + 1);
        if (node->right)
            print_tree(node->right, depth + 1);
    }
}

int main(void)
{
    lex_t *lctx = lex_init("test.c");
    vec_token_t *tokens = vec_token_init();

    /* Construct lexemes from a file */
    token_t *tok;
    do {
        tok = lex_token(lctx);
        vec_token_push(tokens, tok);
    } while (tok->type != TOK_EOF);

    printf("\nTOKEN STREAM:\n-----------\n");
    for (size_t i = 0; i < tokens->len; ++i) {
        token_t *t = tokens->data[i];
        printf("%s: ", __token_names[t->type]);
        printf("%s\n", t->is_literal ? t->literal : "");
    }

    lex_free(lctx);

    /* Channel lexemes through parser */
    rdp_t *rctx = rdp_init(tokens->data, tokens->len);

    /* Get a root node for an AST */
    printf("\nAST:\n-----------\n");
    node_t *root = rdp_generate_ast(rctx);

    print_tree(root, 0);
    rdp_free(rctx);

    printf("\nEVAL:\n----------\n");
    eval_t *ectx = eval_init(root);

    printf("%ld\n", eval_compute(ectx));

    eval_free(ectx);

    return 0;
}
