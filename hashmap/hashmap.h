#ifndef SMALLC_HASHMAP_H
#define SMALLC_HASHMAP_H

#include "../types/types.h"
#include "../memory/memory.h"
#include "../string/string.h"

/* load factor: rehash when count/n_buckets >= 70% */
#define HM_LOAD_NUM 70
#define HM_LOAD_DEN 100

typedef struct HmEntry {
    char           *key;
    void           *val;
    struct HmEntry *next;
} HmEntry;

typedef struct {
    HmEntry **buckets;
    usize     n_buckets;
    usize     count;
    usize     val_size;
    Arena    *arena;
} HashMap;

/* val_size: bytes per value — copied by value on insert/update */
int   hm_init(HashMap *m, usize n_buckets, usize val_size, Arena *a);
int   hm_set (HashMap *m, const char *key, const void *val);
void *hm_get (HashMap *m, const char *key);   /* NULL if not found */
int   hm_del (HashMap *m, const char *key);   /* TRUE if deleted */
void  hm_free(HashMap *m);

/* iterate: cb returns 0 to stop, 1 to continue */
void  hm_each(HashMap *m, int (*cb)(const char *key, void *val, void *ctx), void *ctx);

/* hash primitives (also usable standalone) */
u32   hm_hash_fnv1a(const void *data, usize len);
u32   hm_hash_str  (const char *s);

#endif /* SMALLC_HASHMAP_H */
