#ifndef EVAL_H
#define EVAL_H

#include "parser.h"

typedef struct {
    node_t *root;
    node_t *curr;
} eval_t;


eval_t* eval_init(node_t *ast);

void eval_free(eval_t *ctx);

long eval_compute(eval_t *ctx);

#endif
