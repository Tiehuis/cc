#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>

#define xmalloc(x)              \
({                              \
    void *__mem = malloc(x);    \
    if (!__mem) abort();        \
    __mem;                      \
})

#define xrealloc(p, x)          \
({                              \
    void *__mem = realloc(p, x);\
    if (!__mem) abort();        \
    __mem;                      \
})

#define xcalloc(x)              \
({                              \
    void *__mem = calloc(x, 1); \
    if (!__mem) abort();        \
    __mem;                      \
})

#define xerror(msg)             \
({                              \
    fprintf(stderr, "%s\n", msg);\
    abort();                    \
})

#endif
