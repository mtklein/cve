// Phase 3: broaden the type matrix and exercise comparison + bitwise.
//
// Tests across {int8, int16, int32, int64, uint8, uint16, uint32, uint64,
//                float, double} × {2, 4, 8, 16}.

#include "cve.h"

#include <cassert>
#include <cstdint>
#include <cstdio>

// ---- arithmetic + indexing for every (T, N) in the matrix ------------------
template <class T, int N>
static void test_basic_arith() {
    using V = cve<T, N>;
    V a = static_cast<T>(3);
    V b = static_cast<T>(2);

    V sum = a + b;
    V dif = a - b;
    V mul = a * b;
    for (int i = 0; i < N; ++i) {
        assert(sum[i] == T(5));
        assert(dif[i] == T(1));
        assert(mul[i] == T(6));
    }

    V s = static_cast<T>(7) + a;     // T + V splat-and-add
    for (int i = 0; i < N; ++i) assert(s[i] == T(10));
}

template <class T, int N>
static void test_basic_arith_int() {
    test_basic_arith<T, N>();
    using V = cve<T, N>;
    V a = static_cast<T>(7);
    V b = static_cast<T>(2);
    V q = a / b;
    V r = a % b;
    for (int i = 0; i < N; ++i) {
        assert(q[i] == T(3));
        assert(r[i] == T(1));
    }
}

// ---- comparisons (return mask vec) ----------------------------------------
template <class T, int N>
static void test_cmp() {
    using V = cve<T, N>;
    using M = cve<cve_mask<T>, N>;
    V a = static_cast<T>(3);
    V b = static_cast<T>(5);

    M eq = (a == a);
    M ne = (a != b);
    M lt = (a <  b);
    M ge = (b >= a);
    for (int i = 0; i < N; ++i) {
        assert(eq[i] == cve_mask<T>(-1));
        assert(ne[i] == cve_mask<T>(-1));
        assert(lt[i] == cve_mask<T>(-1));
        assert(ge[i] == cve_mask<T>(-1));
    }
    M neq = (a == b);
    for (int i = 0; i < N; ++i) assert(neq[i] == 0);
}

// ---- bitwise (integer only) -----------------------------------------------
template <class T, int N>
static void test_bitwise() {
    using V = cve<T, N>;
    V a = static_cast<T>(0b1100);
    V b = static_cast<T>(0b1010);

    V andv = a & b;
    V orv  = a | b;
    V xorv = a ^ b;
    V notv = ~a;
    for (int i = 0; i < N; ++i) {
        assert(andv[i] == T(0b1000));
        assert(orv[i]  == T(0b1110));
        assert(xorv[i] == T(0b0110));
        assert(notv[i] == T(~T(0b1100)));
    }

    V shl = a << T(2);
    V shr = a >> T(1);
    for (int i = 0; i < N; ++i) {
        assert(shl[i] == T(0b1100 << 2));
        assert(shr[i] == T(0b1100 >> 1));
    }
}

template <class T, int N>
static void run_int() {
    test_basic_arith_int<T, N>();
    test_cmp<T, N>();
    test_bitwise<T, N>();
}

template <class T, int N>
static void run_float() {
    test_basic_arith<T, N>();
    test_cmp<T, N>();
    // float division
    using V = cve<T, N>;
    V a = static_cast<T>(10);
    V b = static_cast<T>(4);
    V q = a / b;
    for (int i = 0; i < N; ++i) assert(q[i] == T(2.5));
}

template <class T>
static void run_int_all_n() {
    run_int<T, 2>();
    run_int<T, 4>();
    run_int<T, 8>();
    run_int<T, 16>();
}

template <class T>
static void run_float_all_n() {
    run_float<T, 2>();
    run_float<T, 4>();
    run_float<T, 8>();
    run_float<T, 16>();
}

int main() {
    run_int_all_n<std::int8_t>();
    run_int_all_n<std::int16_t>();
    run_int_all_n<std::int32_t>();
    run_int_all_n<std::int64_t>();
    run_int_all_n<std::uint8_t>();
    run_int_all_n<std::uint16_t>();
    run_int_all_n<std::uint32_t>();
    run_int_all_n<std::uint64_t>();
    run_float_all_n<float>();
    run_float_all_n<double>();

    std::printf("test_types [%s]: OK\n", CVE_BACKEND_NAME);
    return 0;
}
