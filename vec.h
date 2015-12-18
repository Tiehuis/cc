/**
 * The following macro generates a vector implementation for a given type.
 * Due to the way this needs to be declared and possible redefinition problems
 * across seperate compilation units, this is only defined file-by-file in
 * each c file it is required.
 *
 * As a result, no public interface can utilize this. This isn't too much of a
 * problem, and only requires that if returning data from a vector, we instead
 * return the data contents and the length manually.
 *
 * These limitations make this method less prone to errors, and also means the
 * interface is more standard, so plugging in seperate lexers and parsers may
 * be a little more straight-forward.
 */

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
static const size_t vec_##typename##_initial_alloc = 128;                    \
                                                                             \
static vec_##typename##_t* vec_##typename##_init(void)                       \
{                                                                            \
    vec_##typename##_t *v = xmalloc(sizeof(vec_##typename##_t));             \
    v->alloc = vec_##typename##_initial_alloc;                               \
    v->data  = xmalloc(v->alloc * sizeof(type));                             \
    v->len   = 0;                                                            \
    return v;                                                                \
}                                                                            \
                                                                             \
static void vec_##typename##_free(vec_##typename##_t *v)                     \
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
static void vec_##typename##_push(vec_##typename##_t *v, type elem)          \
{                                                                            \
    __vec_##typename##_try_resize;                                           \
    v->data[v->len++] = elem;                                                \
}                                                                            \
                                                                             \
static type vec_##typename##_pop(vec_##typename##_t *v)                      \
{                                                                            \
    return v->data[--v->len];                                                \
}                                                                            \
                                                                             \
static char __allow_trailing_comma_##typename

#endif
