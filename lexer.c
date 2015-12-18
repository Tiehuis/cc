/**
 * lexer.c
 *
 * Lexes an input file and generates a stream of tokens.
 */

#include <ctype.h>
#include <stdio.h>
#include "compiler.h"
#include "token.h"
#include "lexer.h"

/* Initialize vec_token_t */
VEC_DECLARE(token_t*, token);

/* Initialize vec_char_t */
VEC_DECLARE(char, char);

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

static int lex_getc(lex_t *ctx)
{
    return fgetc(ctx->fd);
}

static void lex_ungetc(lex_t *ctx, char c)
{
    ungetc(c, ctx->fd);
}

/**
 * The 'lex_mk*' functions allocate memory or 'make' a particular token.
 */

static token_t* lex_mkspecial(int type)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 0;
    return token;
}

static token_t* lex_mkkw(int type)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 0;
    return token;
}

static token_t* lex_mknumber(int type, char *number)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 1;
    token->literal = number;
    return token;
}

static token_t* lex_mkident(int type, char *identifier)
{
    token_t *token = xmalloc(sizeof(token_t));
    token->type = type;
    token->is_literal = 1;
    token->literal = identifier;
    return token;
}

/**
 * The 'lex_ch*' functions 'check' the incoming tokens for expected results
 * and call 'lex_mk' functions from them.
 */

static token_t* lex_chnumber(lex_t *ctx, char value)
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

static token_t* lex_chident(lex_t *ctx, char value)
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

static int lex_skip_space(lex_t *ctx)
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

/* Return the next found token. This could be implemented as a FSM using switch
 * statements and may at some point, but with a file-backed, char-by-char read
 * approach it makes consuming string-literals and numbers a little less
 * convenient. */
token_t* lex_token(lex_t *ctx)
{
    // Skip all spaces and comments and return space token if successful
    if (lex_skip_space(ctx))
        return lex_mkspecial(TOK_SPACE);

    // Read a character and proceed case by case
    int c = lex_getc(ctx);

    switch (c) {
    case '+':
        return lex_mkkw(TOK_PLUS);
    case '-':
        return lex_mkkw(TOK_MINUS);
    case '/':
        return lex_mkkw(TOK_DIV);
    case '*':
        return lex_mkkw(TOK_MULTIPLY);
    case '%':
        return lex_mkkw(TOK_MOD);
    case '=':
        return lex_mkkw(TOK_EQUALS);
    case '&':
        return lex_mkkw(TOK_BWAND);
    case '|':
        return lex_mkkw(TOK_BWOR);
    case '^':
        return lex_mkkw(TOK_BWXOR);
    case ';':
        return lex_mkkw(TOK_SEMICOLON);
    case '0' ... '9':
        return lex_chnumber(ctx, c);
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
        return lex_chident(ctx, c);
    case EOF:
        return lex_mkspecial(TOK_EOF);
    }

    return lex_mkspecial(TOK_EOF);
}
