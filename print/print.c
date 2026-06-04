#include "print.h"

/* =========================
   syscall
   ========================= */
long sys_write(long fd, const void *buf, u64 count)
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

/* =========================
   helpers
   ========================= */

static void putc(char c)
{
    sys_write(1, &c, 1);
}

/* =========================
   string
   ========================= */

void print(const char *s)
{
    if (!s) return;

    u64 len = sc_strlen(s);
    sys_write(1, s, len);
}

/* =========================
   decimal
   ========================= */

static void print_u64(u64 v)
{
    char buf[21];
    u64 i = 20;

    buf[i] = '\0';

    if (v == 0) {
        putc('0');
        return;
    }

    while (v > 0) {
        buf[--i] = '0' + (v % 10);
        v /= 10;
    }

    sys_write(1, &buf[i], 20 - i);
}

void printd(i64 v)
{
    if (v < 0) {
        putc('-');
        print_u64((u64)(-v));
    } else {
        print_u64((u64)v);
    }
}

/* =========================
   hex
   ========================= */

void printh(u64 v)
{
    const char *hex = "0123456789abcdef";
    char buf[16];
    u64 i = 16;

    if (v == 0) {
        print("0x0");
        return;
    }

    while (v && i) {
        buf[--i] = hex[v & 0xF];
        v >>= 4;
    }

    print("0x");
    sys_write(1, &buf[i], 16 - i);
}

/* =========================
   binary
   ========================= */

void printb(u64 v)
{
    char buf[64];
    u64 i = 64;

    if (v == 0) {
        print("0b0");
        return;
    }

    while (v && i) {
        buf[--i] = (v & 1) ? '1' : '0';
        v >>= 1;
    }

    print("0b");
    sys_write(1, &buf[i], 64 - i);
}
