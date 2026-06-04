#include <smallc/print.h>
#include <smallc/types.h>

u8 add(u8 x, u8 y){
    return x+y;
}

void print_example(void)
{
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

int main(){
    print_example();
    u8 a = 20;
    u8 b = 40;
    printd(a);
    print(" + ");
    printd(b);
    print(" = ");
    printd(add(a,b));

    return 0;
}
