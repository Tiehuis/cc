#ifndef PARSER_H
#define PARSER_H

#include "compiler.h"
#include "lexer.h"
#include "vec.h"

enum {
    LITERAL
};

typedef struct node_s {
    int type;
    int id;
    union {
        /* char, int, long */
        long value;

        /* Binary operator */
        struct {
            struct node_s *left;
            struct node_s *right;
        };

        /* Unary operator */
        struct {
            struct node_s *operand;
        };
    };
} node_t;

VEC_DECLARE(node_t, node);

/* Stores the context of this recursive descent parser, in this case a stream
 * of tokens. */
typedef struct {
    vec_token_t *tokens;
    size_t position;
} rdp_t;

#endif
