#include "memory.h"

/* ============================================================
   INTERNAL CONFIG
   ============================================================ */
#define SMALL_ALLOC_LIMIT 4096
#define ARENA_CAP         (1024 * 1024)

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

/* ============================================================
   RAW SYSCALLS (x86-64 Linux)
   mmap  = syscall 9
   munmap = syscall 11
   ============================================================ */
#define PROT_READ_WRITE  0x3          /* PROT_READ | PROT_WRITE */
#define MAP_PRIVATE_ANON 0x22         /* MAP_PRIVATE | MAP_ANONYMOUS */
#define MAP_FAILED_VAL   ((void *)-1)

static void *sc_mmap(usize size) {
    void *ret;
    register long r10 __asm__("r10") = MAP_PRIVATE_ANON;
    register long r8  __asm__("r8")  = -1;  /* fd */
    register long r9  __asm__("r9")  =  0;  /* offset */
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(9),               /* mmap */
          "D"((void *)0),       /* addr   = NULL  (rdi) */
          "S"(size),            /* length          (rsi) */
          "d"(PROT_READ_WRITE), /* prot            (rdx) */
          "r"(r10),             /* flags           (r10) */
          "r"(r8),              /* fd              (r8)  */
          "r"(r9)               /* offset          (r9)  */
        : "memory", "rcx", "r11"
    );
    return ret;
}

static long sc_munmap(void *ptr, usize size) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(11),                 /* munmap */
          "D"(ptr),
          "S"(size)
        : "memory", "rcx", "r11"
    );
    return ret;
}

/* ============================================================
   PAGE ALLOCATOR (internal)
   ============================================================ */
static void *page_alloc(usize size) {
    usize aligned = (size + PAGE_SIZE - 1) & ~(usize)(PAGE_SIZE - 1);
    void *p = sc_mmap(aligned);
    return (p == MAP_FAILED_VAL) ? NULL : p;
}

static void page_free(void *ptr, usize size) {
    if (!ptr || size == 0) return;
    usize aligned = (size + PAGE_SIZE - 1) & ~(usize)(PAGE_SIZE - 1);
    sc_munmap(ptr, aligned);
}

/* ============================================================
   HELPERS  (no stdlib)
   ============================================================ */
static void sc_memset(void *dst, u8 val, usize n) {
    u8 *p = (u8 *)dst;
    while (n--) *p++ = val;
}

/* ============================================================
   ARENA
   ============================================================ */
#define HEADER_SIZE (sizeof(usize))

static void  *arena_base = NULL;
static usize  arena_pos  = 0;

static void arena_init(void) {
    if (arena_base) return;
    arena_base = page_alloc(ARENA_CAP);
    arena_pos  = 0;
}

/* ============================================================
   LARGE ALLOCATION LIMIT
   ============================================================ */
static usize large_allocated = 0;
static usize large_limit     = 1 * MB;

void mem_limit(usize bytes) {
    large_limit = bytes;
}

/* ============================================================
   PUBLIC API
   ============================================================ */
void *malloc(usize size) {
    if (size == 0) return NULL;

    if (size > SMALL_ALLOC_LIMIT) {
        usize total = size + HEADER_SIZE;
        if (large_allocated + total > large_limit) return NULL;
        void *raw = page_alloc(total);
        if (!raw) return NULL;
        *(usize *)raw = total;
        large_allocated += total;
        return (u8 *)raw + HEADER_SIZE;
    }

    arena_init();
    if (!arena_base) return NULL;

    usize p = align_up(arena_pos, 8);
    usize total = HEADER_SIZE + size;
    if (p + total > ARENA_CAP) return NULL;

    *(usize *)((u8 *)arena_base + p) = 0;    /* 0 = arena-owned */
    void *ptr = (u8 *)arena_base + p + HEADER_SIZE;
    arena_pos = p + total;
    return ptr;
}

void *calloc(usize n, usize size) {
    usize total = n * size;
    void *ptr = malloc(total);
    if (ptr) sc_memset(ptr, 0, total);
    return ptr;
}

void free(void *ptr) {
    if (!ptr) return;
    usize *hdr = (usize *)((u8 *)ptr - HEADER_SIZE);
    if (*hdr == 0) return;
    large_allocated -= *hdr;
    page_free(hdr, *hdr);
}

void *realloc(void *ptr, usize size) {
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }
    void *new = malloc(size);
    if (!new) return NULL;
    usize *hdr = (usize *)((u8 *)ptr - HEADER_SIZE);
    usize old_size = (*hdr == 0) ? SMALL_ALLOC_LIMIT : *hdr - HEADER_SIZE;
    usize copy = size < old_size ? size : old_size;
    u8 *s = (u8 *)ptr, *d = (u8 *)new;
    while (copy--) *d++ = *s++;
    free(ptr);
    return new;
}
