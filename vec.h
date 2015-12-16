#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include "compiler.h"

#define VEC_DECLARE(type, typename)                                          \
                                                                             \
typedef struct {                                                             \
    type *data;                                                              \
    size_t alloc;                                                            \
    size_t len;                                                              \
} vec_##typename##_t;                                                        \
                                                                             \
const size_t vec_##typename##_initial_alloc = 128;                           \
                                                                             \
vec_##typename##_t* vec_##typename##_init(void)                              \
{                                                                            \
    vec_##typename##_t *v = xmalloc(sizeof(vec_##typename##_t));             \
    v->alloc = vec_##typename##_initial_alloc;                               \
    v->data  = xmalloc(v->alloc * sizeof(type));                             \
    v->len   = 0;                                                            \
    return v;                                                                \
}                                                                            \
                                                                             \
void vec_##typename##_free(vec_##typename##_t *v)                            \
{                                                                            \
    free(v->data);                                                           \
    free(v);                                                                 \
}                                                                            \
                                                                             \
static void __vec_##typename##_try_resize(vec_##typename##_t *v)             \
{                                                                            \
    if (v->len + 1 <= v->alloc)                                              \
        return;                                                              \
                                                                             \
    v->alloc *= 2;                                                           \
    v->data  = xrealloc(v->data, v->alloc * sizeof(type));                   \
}                                                                            \
                                                                             \
void vec_##typename##_push(vec_##typename##_t *v, type elem)                 \
{                                                                            \
    __vec_##typename##_try_resize;                                           \
    v->data[v->len++] = elem;                                                \
}                                                                            \
                                                                             \
type vec_##typename##_pop(vec_##typename##_t *v)                             \
{                                                                            \
    return v->data[--v->len];                                                \
}                                                                            \
                                                                             \
static char __allow_trailing_comma_##typename

#endif
