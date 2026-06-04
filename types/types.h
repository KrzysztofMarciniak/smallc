#ifndef SMALLC_TYPES_H
#define SMALLC_TYPES_H

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef signed long long    i64;
typedef float               f32;
typedef double              f64;
typedef u64                 usize;
typedef i64                 isize;

#define NULL  ((void*)0)
#define TRUE  1
#define FALSE 0

static inline usize align_up(usize x, usize align) {
    return (x + align - 1) & ~(align - 1);
}

#endif /* SMALLC_TYPES_H */
