/**
 * lexer.c
 *
 * Lexes an input file and generates a stream of tokens.
 */

#include <ctype.h>
#include <stdio.h>
#include "compiler.h"
#include "lexer.h"

token_t __lex_eof_token_store = { TOK_EOF , 0};
token_t *lex_eof_token = &__lex_eof_token_store;

token_t __lex_space_token_store = { TOK_SPACE, 0 };
token_t *lex_space_token = &__lex_space_token_store;

lex_t* lex_init(char *filename)
{
    lex_t *ctx = xmalloc(sizeof(lex_t));
    ctx->fd = fopen(filename, "rb");
    return ctx;
}

void lex_free(lex_t *ctx)
{
    fclose(ctx->fd);
    free(ctx);
}

int lex_getc(lex_t *ctx)
{
    return fgetc(ctx->fd);
}

void lex_ungetc(lex_t *ctx, char c)
{
    ungetc(c, ctx->fd);
}

token_t* lex_mkkw(int type)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 0;
    return token;
}

token_t* lex_mknumber(int type, char *number)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 1;
    token->literal = number;
    return token;
}

token_t* lex_mkident(int type, char *identifier)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 1;
    token->literal = identifier;
    return token;
}

token_t* lex_chnumber(lex_t *ctx, char value)
{
    vec_char_t *number = vec_char_init();
    vec_char_push(number, value);

    int c;
    while ((c = lex_getc(ctx)) != EOF) {
        if (!isdigit(c)) {
            lex_ungetc(ctx, c);
            break;
        }
        vec_char_push(number, c);
    }

    /* This will leak memory for the moment, not important just yet but beware */
    vec_char_push(number, '\0');
    return lex_mknumber(TOK_NUMBER, number->data);
}

token_t* lex_chident(lex_t *ctx, char value)
{
    vec_char_t *identifier = vec_char_init();
    vec_char_push(identifier, value);

    int c;
    while ((c = lex_getc(ctx)) != EOF) {
        if (!isalpha(c) || c == '_') {
            lex_ungetc(ctx, c);
            break;
        }
        vec_char_push(identifier, c);
    }

    /* This will leak memory for the moment, not important just yet but beware */
    vec_char_push(identifier, '\0');
    return lex_mkident(TOK_IDENT, identifier->data);
}

int lex_skip_space(lex_t *ctx)
{
    int c = lex_getc(ctx);

    if (c == EOF)
        return 0;

    if (isspace(c))
        return 1;

    /* Non-whitespace */
    lex_ungetc(ctx, c);
    return 0;
}

/* Return the next found token */
token_t* lex_token(lex_t *ctx)
{
    // Skip all spaces and comments and return space token if successful
    if (lex_skip_space(ctx))
        return lex_space_token;

    // Read a character and proceed case by case
    int c = lex_getc(ctx);

    switch (c) {
    case '+':
        return lex_mkkw(TOK_PLUS);
    case '=':
        return lex_mkkw(TOK_EQUALS);
    case ';':
        return lex_mkkw(TOK_SEMICOLON);
    case '0' ... '9':
        return lex_chnumber(ctx, c);
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
        return lex_chident(ctx, c);
    case EOF:
        return NULL;
    }

    return NULL;
}

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
        printf("%s: ", token_names[t->type]);
        printf("%s\n", t->is_literal ? t->literal : "");
    }

    lex_free(ctx);
}
