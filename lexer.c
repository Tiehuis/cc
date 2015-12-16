/**
 * lexer.c
 *
 * Lexes an input file and generates a stream of tokens.
 */

#include <ctype.h>
#include <stdio.h>
#include "compiler.h"
#include "vec.h"

#define X(x) char *x = #x
X(TOK_IDENT);
X(TOK_PLUS);
X(TOK_SEMICOLON);
X(TOK_EQUALS);
X(TOK_NUMBER);
#undef X

typedef struct {
    int type;
    char *name;
    union {
        int id;
        char *literal;
    };
} token_t;

/* Initialize vec_token_t */
VEC_DECLARE(token_t*, token);

/* Initialize vec_char_t/string */
VEC_DECLARE(char, char);

token_t __lex_eof_token_store = { 0, "TOK_EOF" };
token_t *lex_eof_token = &__lex_eof_token_store;

token_t __lex_space_token_store = { 0, "TOK_SPACE" };
token_t *lex_space_token = &__lex_space_token_store;

typedef struct {
    FILE *fd;
} lex_t;

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

token_t* lex_mkkw(char *type)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = 0;    // This isn't set yet, but the arg will soon be type
    token->name = type;
    return token;
}

token_t* lex_mknumber(char *type, char *number)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = 0;
    token->name = type;
    token->literal = number;
    return token;
}

token_t* lex_mkident(char *type, char *identifier)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = 0;
    token->name = type;
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

    token_t *tok;
    while ((tok = lex_token(ctx)) != NULL) {
        printf("%s, ", tok->name);
    }

    lex_free(ctx);
}
