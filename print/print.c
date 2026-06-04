#include "print.h"

static long sys_write(long fd, const void *buf, u64 count)
{
    long ret;

    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "a"(1L),
          "D"(fd),
          "S"(buf),
          "d"(count)
        : "rcx", "r11", "memory"
    );

    return ret;
}

void printn(const char *s, u64 len)
{
    if (!s || !len)
        return;

    sys_write(1, s, len);
}

void print(const char *s)
{
    if (!s)
        return;

    printn(s, sc_strlen(s));
}

void println(const char *s)
{
    print(s);
    sys_write(1, "\n", 1);
}
