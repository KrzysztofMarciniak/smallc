# print

No-stdlib print utilities for x86-64 Linux. Writes directly to stdout via raw `write`.

## How it works

All output goes through `sys_write(1, buf, len)` — a raw syscall wrapper around `write(2)`. No buffering.

## API

```c
void print(const char *s);   /* string          */
void printd(i64 v);          /* signed decimal  */
void printh(u64 v);          /* hex  (0x...)    */
void printb(u64 v);          /* binary (0b...)  */
```

## Example

```c
print("hello\n");     // hello
printd(123);          // 123
printd(-42);          // -42
printh(0xdeadbeef);   // 0xdeadbeef
printb(4);            // 0b100
```

## Output formats

| Function | Example output |
|----------|---------------|
| `print`  | `hello world` |
| `printd` | `-42`         |
| `printh` | `0xdeadbeef`  |
| `printb` | `0b11010`     |

## Dependencies

- `string.h` — for `sc_strlen` used in `print`
- `types.h` — for `u64`, `i64`

## Notes

- `print` does not append a newline — pass `"\n"` explicitly.
- `printh` and `printb` print the minimal digits — no zero-padding.
- Compiling with `-nostdlib` required.
