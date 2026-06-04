# types

Primitive type definitions for smallc. No dependencies.

## Types

### Unsigned integers
| Type  | Size    |
|-------|---------|
| `u8`  | 8-bit   |
| `u16` | 16-bit  |
| `u32` | 32-bit  |
| `u64` | 64-bit  |

### Signed integers
| Type  | Size    |
|-------|---------|
| `i8`  | 8-bit   |
| `i16` | 16-bit  |
| `i32` | 32-bit  |
| `i64` | 64-bit  |

### Floats
| Type  | Size    |
|-------|---------|
| `f32` | 32-bit  |
| `f64` | 64-bit  |

### Size types
| Type    | Alias  |
|---------|--------|
| `usize` | `u64`  |
| `isize` | `i64`  |

## Constants

```c
NULL   // (void *)0
TRUE   // 1
FALSE  // 0
```

## Utilities

```c
usize align_up(usize x, usize align);
```

Rounds `x` up to the next multiple of `align`. `align` must be a power of two.

```c
align_up(5, 8);   // 8
align_up(8, 8);   // 8
align_up(9, 8);   // 16
```
