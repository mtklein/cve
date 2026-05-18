# cve

C++20 header-only polyfill for clang's `ext_vector_type` vectors,
plus GLSL swizzle accessors. Single file: `cve.h`.

```cpp
#include "cve.h"
using f4 = cve<float, 4>;

f4 fma(f4 a, f4 b, f4 c) {
    return a * b + c;
}
```

Elements: `{int,uint}{8,16,32,64}_t`, `float`, `double`.
Widths: N = 2, 3, 4, 8, 16. Named accessors and 1/2/3/4-letter swizzles
on N = 2, 3, 4.

## Backends

| Backend       | Storage                                     | Selected for  |
|---------------|---------------------------------------------|---------------|
| native        | `__attribute__((ext_vector_type(N)))`       | clang         |
| vector_size   | `__attribute__((vector_size(N*sizeof(T))))` | gcc           |
| portable      | `std::array<T, N>`                          | else          |

Storage byte count rounds up to a power of 2 (`vector_size` and
`alignas` require it); `cve<float, 3>` is backed by 16 bytes and
exposes lanes 0..2. `-DCVE_FORCE_PORTABLE` overrides the selection.
All three pass the same test suite.

## constexpr

Arithmetic, comparisons, bitwise, shifts, shuffle, convert, and
`operator[]` are constexpr on all three backends. The math helpers
(`cve_abs`, `cve_ceil`, `cve_floor`, `cve_fma`, `cve_max`, `cve_min`, `cve_round`, `cve_sqrt`) are runtime.

Named swizzle accessors (`v.x`, `v.xy`, ...) are not constexpr on the
wrapper backends: they are empty subobjects at offset 0 via
`[[no_unique_address]]`, accessed through
`reinterpret_cast<storage_t*>(this)`, and `reinterpret_cast` cannot
appear in constant expressions.

## ABI and codegen

Instruction count for `fma(a, b, c) = a*b + c` on AArch64, at `-O0`, `-O1`, and `-O2`:

| backend                                             | -O0 | -O1 | -O2 |
|-----------------------------------------------------|----:|----:|----:|
| clang native (`ext_vector_type`)                    |  10 |   3 |   3 |
| gcc default (`vector_size`)                         |  44 |   3 |   3 |
| gcc `-DCVE_FORCE_PORTABLE` (`std::array<float, 4>`) | 462 |  70 |  36 |

## Compile time

`-O1 -Werror`, median of 5 hyperfine runs, Apple M4:

| file                | native (clang) | portable (clang) | gcc (vector_size) |
|---------------------|---------------:|-----------------:|------------------:|
| `test_arith.cc`     |         147 ms |   230 ms (1.57x) |    326 ms (2.22x) |
| `test_swizzle.cc`   |         149 ms |   280 ms (1.89x) |    420 ms (2.83x) |
| `test_loadstore.cc` |         327 ms |   382 ms (1.17x) |    321 ms (0.98x) |
| `test_types.cc`     |         178 ms |  1340 ms (7.51x) |  1921 ms (10.77x) |

`test_types.cc` instantiates 10 element types × 5 widths and evaluates
all operators through static_assert. At `-O0`, `-O1`, and `-O2`:

| `-O`  | native |        portable |
|-------|-------:|----------------:|
| `-O0` | 181 ms | 1352 ms (7.47x) |
| `-O1` | 179 ms | 1346 ms (7.52x) |
| `-O2` | 180 ms | 1349 ms (7.48x) |

## Building

    ninja
