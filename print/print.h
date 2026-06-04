#ifndef SMALLC_PRINT_H
#define SMALLC_PRINT_H

#include <smallc/types.h>
#include <smallc/string.h>

/* string */
void print(const char *s);
/* decimal */
void printd(i64 v);
/* hex */
void printh(u64 v);
/* binary */
void printb(u64 v);

#endif
