#include "string.h"

usize sc_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (usize)(p - s);
}

int sc_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int sc_strncmp(const char *a, const char *b, usize n) {
    while (n-- && *a && (*a == *b)) { a++; b++; }
    if (!n) return 0;  /* note: n wrapped to ULLONG_MAX after last decrement */
    return (unsigned char)*a - (unsigned char)*b;
}

char *sc_strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

char *sc_strncpy(char *dst, const char *src, usize n) {
    char *d = dst;
    while (n && (*d++ = *src++)) n--;
    while (n--) *d++ = '\0';
    return dst;
}

char *sc_strcat(char *dst, const char *src) {
    char *d = dst;
    while (*d) d++;
    while ((*d++ = *src++));
    return dst;
}

char *sc_strchr(const char *s, char c) {
    for (; *s; s++)
        if (*s == c) return (char*)s;
    return NULL;
}

char *sc_strrchr(const char *s, char c) {
    const char *last = NULL;
    for (; *s; s++)
        if (*s == c) last = s;
    return (char*)last;
}

char *sc_strdup_buf(char *dst, usize dst_cap, const char *src) {
    usize len = sc_strlen(src);
    if (len + 1 > dst_cap) return NULL;
    sc_strcpy(dst, src);
    return dst;
}
