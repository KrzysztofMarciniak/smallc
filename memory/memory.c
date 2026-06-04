#include "memory.h"

/* ============================================================
   platform page allocator
   ============================================================ */

#if defined(__linux__) && defined(__x86_64__)

/* mmap syscall: nr=9, munmap: nr=11 */
void *sc_page_alloc(usize size) {
    usize sz = align_up(size, 4096);
    void *ret;
    register long r10 __asm__("r10") = 0x22; /* MAP_PRIVATE|MAP_ANONYMOUS */
    register long r8  __asm__("r8")  = -1;
    register long r9  __asm__("r9")  = 0;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(9), "D"((void*)0), "S"(sz), "d"(3), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory"
    );
    return ret;
}

void sc_page_free(void *ptr, usize size) {
    usize sz = align_up(size, 4096);
    __asm__ volatile (
        "syscall"
        : : "a"(11), "D"(ptr), "S"(sz)
        : "rcx", "r11", "memory"
    );
}

#else
/* portable fallback — requires libc mmap but no other stdlib */
#include <sys/mman.h>
void *sc_page_alloc(usize size) {
    usize sz = align_up(size, 4096);
    return mmap(NULL, sz, PROT_READ|PROT_WRITE,
                MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}
void sc_page_free(void *ptr, usize size) {
    munmap(ptr, align_up(size, 4096));
}
#endif

/* ============================================================
   raw memory ops
   ============================================================ */

void sc_memset(void *dst, u8 val, usize n) {
    u8 *d = (u8*)dst;
    while (n--) *d++ = val;
}

void sc_memcpy(void *dst, const void *src, usize n) {
    u8 *d       = (u8*)dst;
    const u8 *s = (const u8*)src;
    while (n--) *d++ = *s++;
}

void sc_memmove(void *dst, const void *src, usize n) {
    u8 *d       = (u8*)dst;
    const u8 *s = (const u8*)src;
    if (d < s)       { while (n--) *d++ = *s++; }
    else if (d > s)  { d += n; s += n; while (n--) *--d = *--s; }
}

int sc_memcmp(const void *a, const void *b, usize n) {
    const u8 *x = (const u8*)a, *y = (const u8*)b;
    while (n--) {
        if (*x != *y) return (int)*x - (int)*y;
        x++; y++;
    }
    return 0;
}

/* ============================================================
   arena
   ============================================================ */

void arena_init(Arena *a, void *buf, usize cap) {
    a->buf = (u8*)buf;
    a->cap = cap;
    a->pos = 0;
}

void *arena_alloc(Arena *a, usize size, usize alignment) {
    usize cur = align_up(a->pos, alignment);
    if (cur + size > a->cap) return NULL;
    a->pos = cur + size;
    return a->buf + cur;
}

void arena_reset(Arena *a) {
    a->pos = 0;
}

int arena_create(Arena *a, usize cap) {
    void *mem = sc_page_alloc(cap);
    if (!mem) return FALSE;
    arena_init(a, mem, cap);
    return TRUE;
}

void arena_destroy(Arena *a) {
    if (a->buf) sc_page_free(a->buf, a->cap);
    a->buf = NULL;
    a->cap = 0;
    a->pos = 0;
}
