#ifndef SMALLC_MEMORY_H
#define SMALLC_MEMORY_H

#include "../types/types.h"

/* ---------- raw ops --------------------------------------- */
void  sc_memset (void *dst, u8 val, usize n);
void  sc_memcpy (void *dst, const void *src, usize n);
void  sc_memmove(void *dst, const void *src, usize n);
int   sc_memcmp (const void *a, const void *b, usize n);

/* ---------- page allocator (mmap / syscall) --------------- */
void *sc_page_alloc(usize size);   /* allocates rounded-up to page boundary */
void  sc_page_free (void *ptr, usize size);

/* ---------- arena (bump allocator) ----------------------- */
typedef struct {
    u8    *buf;
    usize  cap;
    usize  pos;
} Arena;

void  arena_init (Arena *a, void *buf, usize cap);
void *arena_alloc(Arena *a, usize size, usize align);
void  arena_reset(Arena *a);

/* convenience: arena backed by a freshly mmap'd region */
int   arena_create (Arena *a, usize cap);   /* returns TRUE on success */
void  arena_destroy(Arena *a);

#endif /* SMALLC_MEMORY_H */
