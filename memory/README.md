# memory

No-stdlib memory allocator for x86-64 Linux. Uses raw syscalls. 

## How it works

Two allocation strategies depending on size:

- **Small (`<= 4096` bytes)** — bumped off a 1MB arena. Frees are no-ops; the arena is reclaimed when the process exits.
- **Large (`> 4096` bytes)** — direct `mmap` per allocation. Freed individually via `munmap`. Subject to `mem_limit`.

Every allocation is prefixed with a hidden `usize` header storing the mapped size so `free` knows how much to unmap.

## API

```c
void *malloc(usize size);
void *calloc(usize n, usize size);
void *realloc(void *ptr, usize size);
void  free(void *ptr);
void  mem_limit(usize bytes);
```

## Size constants

```c
KB  // 1024
MB  // 1024 * KB
GB  // 1024 * MB
```

## mem_limit

Large allocations are capped at **1MB by default**. Once the limit is hit, `malloc` returns `NULL` instead of growing unbounded.

```c
mem_limit(4 * MB);  /* raise limit to 4MB */
mem_limit(256 * KB);  /* lower limit to 256KB */
```

## Example

```c
// small — arena
Vec3 *v = malloc(sizeof(Vec3));
v->x = 10;
free(v);  /* no-op, arena-owned */

// large — direct mmap
void *buf = malloc(8 * KB);
free(buf);  /* munmapped immediately */

// zero-initialized
u8 *data = calloc(16, sizeof(u8));

// resize
data = realloc(data, 32 * sizeof(u8));
```

## Notes

- Compiling with `-nostdlib` is required — `malloc`/`free` will clash with libc otherwise.
- Arena allocations cannot be individually reclaimed. If you need bulk reset, track `arena_pos` externally or add an `arena_reset()`.
- Double-free on large allocations is undefined behavior. Use a `FREE(p)` macro to null the pointer after freeing:

```c
#define FREE(p) (free(p), (p) = NULL)
```
