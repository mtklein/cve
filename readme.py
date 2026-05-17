#!/usr/bin/env python3

import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

CLANG = "/opt/homebrew/opt/llvm/bin/clang++"
GCC   = "/opt/homebrew/bin/g++-15"

WARNS_CLANG = [
    "-Weverything",
    "-Wno-c++98-compat",
    "-Wno-c++98-compat-pedantic",
    "-Wno-pre-c++14-compat",
    "-Wno-pre-c++17-compat",
    "-Wno-pre-c++20-compat",
    "-Wno-pre-c++20-compat-pedantic",
    "-Wno-poison-system-directories",
]
WARNS_GCC = ["-Wall", "-Wextra", "-Wpedantic"]

COMPILE_TIME_OPT   = "-O1"
COMPILE_TIME_FLAGS = ["-Werror"]
HYPERFINE_WARMUP   = 1
HYPERFINE_RUNS     = 5
HYPERFINE_FIELD    = "median"
CODEGEN_LEVELS     = ["-O0", "-O1", "-O2"]
COMPILE_TIME_FILES = [
    "tests/test_arith.cc",
    "tests/test_swizzle.cc",
    "tests/test_loadstore.cc",
    "tests/test_types.cc",
]

def scrubbed_instructions(cxx, source, opt, defines):
    cmd = [cxx, "-std=c++20", opt, "-S", source, "-o", "-"] + defines
    p = subprocess.run(cmd, capture_output=True, text=True, check=True)
    scrub = subprocess.run(
        ["python3", "codegen/scrub.py"],
        input=p.stdout, capture_output=True, text=True, check=True)
    return sum(1 for line in scrub.stdout.splitlines() if line.startswith("\t"))

def hyperfine_ms(cmd):
    with tempfile.NamedTemporaryFile(mode="r", suffix=".json", delete=False) as f:
        path = f.name
    try:
        subprocess.run(
            ["hyperfine", "--warmup", str(HYPERFINE_WARMUP), "--runs", str(HYPERFINE_RUNS),
             "--export-json", path, "--", " ".join(cmd)],
            capture_output=True, text=True, check=True)
        with open(path) as f:
            data = json.load(f)
    finally:
        os.unlink(path)
    return data["results"][0][HYPERFINE_FIELD] * 1000

def compile_cmd(cxx, source, opt, defines, warns):
    return [cxx, "-std=c++20", opt, "-g"] + COMPILE_TIME_FLAGS + warns + defines + [
        "-c", source, "-o", "/tmp/cve_readme.o"]

def render_table(headers, rows, aligns):
    cols = list(zip(*([headers] + rows)))
    widths = [max(len(c) for c in col) for col in cols]
    def pad(cell, w, a):
        return cell.rjust(w) if a == "r" else cell.ljust(w)
    def fmt_row(row):
        return "| " + " | ".join(pad(c, widths[i], aligns[i]) for i, c in enumerate(row)) + " |"
    sep = "|" + "|".join(
        ("-" * (widths[i] + 1) + ":") if aligns[i] == "r" else ("-" * (widths[i] + 2))
        for i in range(len(headers))) + "|"
    return "\n".join([fmt_row(headers), sep] + [fmt_row(r) for r in rows])

def list_phrase(items, quote="`"):
    """['-O0','-O1','-O2'] -> '`-O0`, `-O1`, and `-O2`'."""
    q = [f"{quote}{x}{quote}" for x in items]
    if len(q) == 1: return q[0]
    if len(q) == 2: return f"{q[0]} and {q[1]}"
    return ", ".join(q[:-1]) + f", and {q[-1]}"

def cpu_label():
    return subprocess.run(
        ["sysctl", "-n", "machdep.cpu.brand_string"],
        capture_output=True, text=True, check=True).stdout.strip()

def test_types_matrix():
    """Read tests/test_types.cc and return (#element types, sorted widths)."""
    with open("tests/test_types.cc") as f:
        src = f.read()
    types = set(re.findall(r'static_assert\(run_(?:int|float)_all_n<([^>]+)>\(\)', src))
    widths = set(int(m) for m in re.findall(r'run_(?:int|float)<T,\s*(\d+)>', src))
    return len(types), sorted(widths)

def math_ops():
    """Return the sorted list of cve_* runtime math helpers defined in cve.h."""
    with open("cve.h") as f:
        src = f.read()
    return sorted(set(re.findall(r'^\s*V\s+(cve_\w+)\(', src, re.M)))

def table_codegen():
    def at_each_O(cxx, defines):
        return [scrubbed_instructions(cxx, "codegen/fma.cc", opt, defines)
                for opt in CODEGEN_LEVELS]
    rows = [
        ["clang native (`ext_vector_type`)",                    at_each_O(CLANG, [])],
        ["gcc default (`vector_size`)",                         at_each_O(GCC,   [])],
        ["gcc `-DCVE_FORCE_PORTABLE` (`std::array<float, 4>`)", at_each_O(GCC,   ["-DCVE_FORCE_PORTABLE"])],
    ]
    return render_table(
        ["backend", *CODEGEN_LEVELS],
        [[label, *(str(n) for n in ns)] for label, ns in rows],
        ["l"] + ["r"] * len(CODEGEN_LEVELS))

def table_compile_per_file():
    rows = []
    for f in COMPILE_TIME_FILES:
        nat  = hyperfine_ms(compile_cmd(CLANG, f, COMPILE_TIME_OPT, [], WARNS_CLANG))
        port = hyperfine_ms(compile_cmd(CLANG, f, COMPILE_TIME_OPT, ["-DCVE_FORCE_PORTABLE"], WARNS_CLANG))
        gcc  = hyperfine_ms(compile_cmd(GCC,   f, COMPILE_TIME_OPT, [], WARNS_GCC))
        rows.append([f"`{os.path.basename(f)}`",
                     f"{nat:.0f} ms",
                     f"{port:.0f} ms ({port/nat:.2f}x)",
                     f"{gcc:.0f} ms ({gcc/nat:.2f}x)"])
    return render_table(
        ["file", "native (clang)", "portable (clang)", "gcc (vector_size)"],
        rows,
        ["l", "r", "r", "r"])

def table_compile_across_O():
    rows = []
    for opt in CODEGEN_LEVELS:
        nat  = hyperfine_ms(compile_cmd(CLANG, "tests/test_types.cc", opt, [], WARNS_CLANG))
        port = hyperfine_ms(compile_cmd(CLANG, "tests/test_types.cc", opt, ["-DCVE_FORCE_PORTABLE"], WARNS_CLANG))
        rows.append([f"`{opt}`", f"{nat:.0f} ms", f"{port:.0f} ms ({port/nat:.2f}x)"])
    return render_table(["`-O`", "native", "portable"], rows, ["l", "r", "r"])

TEMPLATE = """\
# cve

C++20 header-only polyfill for clang's `ext_vector_type` vectors,
plus GLSL swizzle accessors. Single file: `cve.h`.

```cpp
#include "cve.h"
using f4 = cve<float, 4>;

f4 fma(f4 a, f4 b, f4 c) {{
    return a * b + c;
}}
```

Elements: `{{int,uint}}{{8,16,32,64}}_t`, `float`, `double`.
Widths: N = {widths_list}. Named accessors and 1/2/3/4-letter swizzles
on N = 2, 3, 4.

## Backends

| Backend       | Storage                                     | Selected for  |
|---------------|---------------------------------------------|---------------|
| native        | `__attribute__((ext_vector_type(N)))`       | clang         |
| vector_size   | `__attribute__((vector_size(N*sizeof(T))))` | gcc           |
| portable      | `std::array<T, N>`                          | else          |

N=3 rounds up to a 4-wide `vector_size` on gcc (the attribute rejects
non-power-of-2 byte counts) and uses lanes 0..2.
`-DCVE_FORCE_PORTABLE` overrides the selection. All three pass the
same test suite.

## constexpr

Arithmetic, comparisons, bitwise, shifts, shuffle, convert, and
`operator[]` are constexpr on all three backends. The math helpers
({math_list}) are runtime.

Named swizzle accessors (`v.x`, `v.xy`, ...) are not constexpr on the
wrapper backends: they are empty subobjects at offset 0 via
`[[no_unique_address]]`, accessed through
`reinterpret_cast<storage_t*>(this)`, and `reinterpret_cast` cannot
appear in constant expressions.

## ABI and codegen

Instruction count for `fma(a, b, c) = a*b + c` on AArch64, at {codegen_levels}:

{codegen}

## Compile time

{compile_label}, {field} of {runs} hyperfine runs, {cpu}:

{compile_per_file}

`test_types.cc` instantiates {n_types} element types × {n_widths} widths and evaluates
all operators through static_assert. At {codegen_levels}:

{compile_across_o}

## Building

    ninja
"""

def main():
    if not shutil.which("hyperfine"):
        sys.exit("hyperfine not found in PATH")
    n_types, widths = test_types_matrix()
    ops = math_ops()
    sys.stdout.write(TEMPLATE.format(
        codegen=table_codegen(),
        compile_per_file=table_compile_per_file(),
        compile_across_o=table_compile_across_O(),
        codegen_levels=list_phrase(CODEGEN_LEVELS),
        compile_label=f"`{COMPILE_TIME_OPT} {' '.join(COMPILE_TIME_FLAGS)}`",
        field=HYPERFINE_FIELD,
        runs=HYPERFINE_RUNS,
        cpu=cpu_label(),
        n_types=n_types,
        n_widths=len(widths),
        widths_list=", ".join(str(w) for w in widths),
        math_list=", ".join(f"`{o}`" for o in ops),
    ))

if __name__ == "__main__":
    main()
