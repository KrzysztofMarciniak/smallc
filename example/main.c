#include <smallc/print.h>
#include <smallc/types.h>
#include <smallc/memory.h>
#include <smallc/string.h>

u8 add(u8 x, u8 y) {
    return x + y;
}

void print_example(void) {
    print("hello");
    print("\n");
    printd(123);
    print("\n");
    printd(-42);
    print("\n");
    printh(0xdeadbeef);
    print("\n");
    printb(4);
    print("\n");
}

/* ============================================================
   MEMORY SYSTEM EXAMPLE
   ============================================================ */
typedef struct {
    u8  x;
    u8  y;
    u8  z;
} Vec3;


void mem_leak(void) {
    while (1) {
        void *p = malloc(8192);
        if (!p) {
            print("limit hit!");
            break;
        }
    }
}

void memory_example(void) {
    /* allocate a Vec3 and wrap it in a ref-counted Pointer */
    Vec3 *v = (Vec3 *)malloc(sizeof(Vec3));
    if (!v) {
        print("malloc failed\n");
        return;
    }
    v->x = 10;
    v->y = 20;
    v->z = 30;
    print("v->x = ");
    printd(v->y);
    print("\n");

    free(v);
}

/* ============================================================
   MAIN
   ============================================================ */
int main() {

    memory_example();
    print_example();
    u8 a = 20;
    u8 b = 40;
    printd(a);
    print(" + ");
    printd(b);
    print(" = ");
    printd(add(a, b));
    print("\n");
    mem_leak();

    return 0;
}
