#include "vector.h"

void vec_init(Vec *v, usize stride, Arena *a) {
    v->data   = NULL;
    v->len    = 0;
    v->cap    = 0;
    v->stride = stride;
    v->arena  = a;
}

static int _vec_grow(Vec *v) {
    usize new_cap = v->cap ? v->cap * 2 : 8;
    usize bytes   = new_cap * v->stride;
    void *mem;

    if (v->arena) {
        mem = arena_alloc(v->arena, bytes, 8);
    } else {
        mem = sc_page_alloc(bytes);
    }
    if (!mem) return FALSE;

    if (v->data)
        sc_memcpy(mem, v->data, v->len * v->stride);

    /* old block leaks if not arena-managed — acceptable for arena use */
    v->data = mem;
    v->cap  = new_cap;
    return TRUE;
}

int vec_push(Vec *v, const void *elem) {
    if (v->len >= v->cap && !_vec_grow(v)) return FALSE;
    sc_memcpy((u8*)v->data + v->len * v->stride, elem, v->stride);
    v->len++;
    return TRUE;
}

void *vec_get(Vec *v, usize i) {
    if (i >= v->len) return NULL;
    return (u8*)v->data + i * v->stride;
}

int vec_set(Vec *v, usize i, const void *elem) {
    if (i >= v->len) return FALSE;
    sc_memcpy((u8*)v->data + i * v->stride, elem, v->stride);
    return TRUE;
}

void vec_pop(Vec *v) {
    if (v->len) v->len--;
}

void vec_swap(Vec *v, usize i, usize j) {
    if (i == j || i >= v->len || j >= v->len) return;
    u8 *a = (u8*)v->data + i * v->stride;
    u8 *b = (u8*)v->data + j * v->stride;
    usize n = v->stride;
    while (n--) {
        u8 tmp = *a;
        *a++ = *b;
        *b++ = tmp;
    }
}

void vec_clear(Vec *v) {
    v->len = 0;
}

void vec_free(Vec *v) {
    if (!v->arena && v->data)
        sc_page_free(v->data, v->cap * v->stride);
    v->data = NULL;
    v->len  = 0;
    v->cap  = 0;
}

void vec_each(Vec *v, int (*cb)(void *elem, usize i, void *ctx), void *ctx) {
    for (usize i = 0; i < v->len; i++)
        if (!cb((u8*)v->data + i * v->stride, i, ctx)) return;
}
