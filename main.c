#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "vec.h"

VEC_DECLARE(token_t*, token);

int _print_t(node_t *node, int is_left, int offset, int depth, char s[20][255])
{
    char b[20];
    int width = 5;

    if (node->id != LITERAL) {
        sprintf(b, "(%s)", __token_names[node->id] + 4);
    }
    else if (node->id == LITERAL) {
        sprintf(b, "(%03ld)", node->value);
    }
    else {
        xerror("failed to print");
    }

    int left = 0, right = 0;

    if (!node->is_term) {
        left  = _print_t(node->left,  1, offset,                depth + 1, s);
        right = _print_t(node->right, 0, offset + left + width, depth + 1, s);
    }

    for (int i = 0; i < width; i++)
        s[depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[depth - 1][offset + left + width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[depth - 1][offset - width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';
    }

    return left + width + right;
}

void print_t(node_t *root)
{
    char s[20][255];
    for (int i = 0; i < 20; i++)
        sprintf(s[i], "%80s", " ");

    _print_t(root, 0, 0, 0, s);

    for (int i = 0; i < 20; i++)
        printf("%s\n", s[i]);
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

    print_t(root);

    rdp_free(rctx);
    return 0;
}

