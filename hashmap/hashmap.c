#include "hashmap.h"

/* ============================================================
   hash
   ============================================================ */

u32 hm_hash_fnv1a(const void *data, usize len) {
    const u8 *p = (const u8*)data;
    u32 h = 2166136261u;
    while (len--) { h ^= *p++; h *= 16777619u; }
    return h;
}

u32 hm_hash_str(const char *s) {
    return hm_hash_fnv1a(s, sc_strlen(s));
}

/* ============================================================
   internal helpers
   ============================================================ */

/* next power of two >= x */
static u32 _pow2_ceil(u32 x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16;
    return x + 1;
}

static HmEntry *_entry_new(HashMap *m, const char *key, const void *val) {
    usize klen  = sc_strlen(key) + 1;
    usize total = sizeof(HmEntry) + klen + m->val_size;
    void *mem   = m->arena
                ? arena_alloc(m->arena, total, 8)
                : sc_page_alloc(total);
    if (!mem) return NULL;

    HmEntry *e = (HmEntry*)mem;
    e->key  = (char*)mem + sizeof(HmEntry);
    e->val  = (u8*)e->key + klen;
    e->next = NULL;
    sc_memcpy(e->key, key, klen);
    sc_memcpy(e->val, val, m->val_size);
    return e;
}

static int _needs_rehash(HashMap *m) {
    return m->count * HM_LOAD_DEN >= m->n_buckets * HM_LOAD_NUM;
}

static int _rehash(HashMap *m) {
    usize new_n = m->n_buckets * 2;
    usize bytes = new_n * sizeof(HmEntry*);
    HmEntry **nb = m->arena
                 ? arena_alloc(m->arena, bytes, 8)
                 : sc_page_alloc(bytes);
    if (!nb) return FALSE;
    sc_memset(nb, 0, bytes);

    for (usize i = 0; i < m->n_buckets; i++) {
        for (HmEntry *e = m->buckets[i]; e; ) {
            HmEntry *next = e->next;
            u32 slot = hm_hash_str(e->key) & (u32)(new_n - 1);
            e->next = nb[slot];
            nb[slot] = e;
            e = next;
        }
    }
    m->buckets   = nb;
    m->n_buckets = new_n;
    return TRUE;
}

/* ============================================================
   public API
   ============================================================ */

int hm_init(HashMap *m, usize n_buckets, usize val_size, Arena *a) {
    n_buckets  = _pow2_ceil((u32)n_buckets);
    usize bytes = n_buckets * sizeof(HmEntry*);
    void *mem   = a ? arena_alloc(a, bytes, 8) : sc_page_alloc(bytes);
    if (!mem) return FALSE;

    sc_memset(mem, 0, bytes);
    m->buckets   = (HmEntry**)mem;
    m->n_buckets = n_buckets;
    m->count     = 0;
    m->val_size  = val_size;
    m->arena     = a;
    return TRUE;
}

int hm_set(HashMap *m, const char *key, const void *val) {
    u32 slot = hm_hash_str(key) & (u32)(m->n_buckets - 1);

    for (HmEntry *e = m->buckets[slot]; e; e = e->next) {
        if (sc_strcmp(e->key, key) == 0) {
            sc_memcpy(e->val, val, m->val_size);
            return TRUE;
        }
    }

    if (_needs_rehash(m) && !_rehash(m)) return FALSE;
    slot = hm_hash_str(key) & (u32)(m->n_buckets - 1);

    HmEntry *e = _entry_new(m, key, val);
    if (!e) return FALSE;

    e->next          = m->buckets[slot];
    m->buckets[slot] = e;
    m->count++;
    return TRUE;
}

void *hm_get(HashMap *m, const char *key) {
    u32 slot = hm_hash_str(key) & (u32)(m->n_buckets - 1);
    for (HmEntry *e = m->buckets[slot]; e; e = e->next)
        if (sc_strcmp(e->key, key) == 0)
            return e->val;
    return NULL;
}

int hm_del(HashMap *m, const char *key) {
    u32 slot      = hm_hash_str(key) & (u32)(m->n_buckets - 1);
    HmEntry **pp  = &m->buckets[slot];
    for (HmEntry *e = *pp; e; pp = &e->next, e = e->next) {
        if (sc_strcmp(e->key, key) == 0) {
            *pp = e->next;
            m->count--;
            /* individual entry memory is reclaimed with the arena */
            return TRUE;
        }
    }
    return FALSE;
}

void hm_free(HashMap *m) {
    if (!m->arena) {
        for (usize i = 0; i < m->n_buckets; i++) {
            for (HmEntry *e = m->buckets[i]; e; ) {
                HmEntry *next = e->next;
                sc_page_free(e, sizeof(HmEntry) + sc_strlen(e->key) + 1 + m->val_size);
                e = next;
            }
        }
        sc_page_free(m->buckets, m->n_buckets * sizeof(HmEntry*));
    }
    m->buckets   = NULL;
    m->n_buckets = 0;
    m->count     = 0;
}

void hm_each(HashMap *m, int (*cb)(const char *key, void *val, void *ctx), void *ctx) {
    for (usize i = 0; i < m->n_buckets; i++)
        for (HmEntry *e = m->buckets[i]; e; e = e->next)
            if (!cb(e->key, e->val, ctx)) return;
}
