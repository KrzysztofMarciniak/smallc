#ifndef SMALLC_STRING_H
#define SMALLC_STRING_H

#include <smallc/types.h>

usize  sc_strlen (const char *s);
int    sc_strcmp (const char *a, const char *b);
int    sc_strncmp(const char *a, const char *b, usize n);
char  *sc_strcpy (char *dst, const char *src);
char  *sc_strncpy(char *dst, const char *src, usize n);
char  *sc_strcat (char *dst, const char *src);
char  *sc_strchr (const char *s, char c);
char  *sc_strrchr(const char *s, char c);

char  *sc_strdup_buf(char *dst, usize dst_cap, const char *src);

#endif /* SMALLC_STRING_H */
