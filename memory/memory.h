#ifndef SMALLC_MEMORY_H
#define SMALLC_MEMORY_H

#include <smallc/types.h>

/* ============================================================
   MEMORY SIZE CONSTANTS
   ============================================================ */
#define KB  (1024)
#define MB  (1024 * KB)
#define GB  (1024 * MB)

/* ============================================================
   C-STYLE ALLOCATOR API
   ============================================================ */
void *malloc(usize size);
void *calloc(usize n, usize size);
void *realloc(void *ptr, usize size);
void  free(void *ptr);
void  mem_limit(usize bytes);



#endif /* SMALLC_MEMORY_H */
