#ifndef SMALLC_VECTOR_H
#define SMALLC_VECTOR_H

#include "../types/types.h"
#include "../memory/memory.h"

typedef struct {
    void  *data;
    usize  len;
    usize  cap;
    usize  stride;  /* element size in bytes */
    Arena *arena;   /* NULL → sc_page_alloc fallback */
} Vec;

void  vec_init   (Vec *v, usize stride, Arena *a);
int   vec_push   (Vec *v, const void *elem);  /* 0 = OOM */
void *vec_get    (Vec *v, usize i);           /* NULL if out of bounds */
int   vec_set    (Vec *v, usize i, const void *elem);
void  vec_pop    (Vec *v);
void  vec_swap   (Vec *v, usize i, usize j);
void  vec_clear  (Vec *v);
void  vec_free   (Vec *v);

/* iterate: cb returns 0 to stop */
void  vec_each   (Vec *v, int (*cb)(void *elem, usize i, void *ctx), void *ctx);

#endif /* SMALLC_VECTOR_H */
