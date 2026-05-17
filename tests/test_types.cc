#include "../cve.h"
#include "test_util.h"

#include <cstdint>

#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wfloat-equal"
#endif

template <class T, std::size_t N>
constexpr bool test_basic_arith() {
    using V = cve<T, N>;
    V a = static_cast<T>(3);
    V b = static_cast<T>(2);

    V sum = a + b;
    V dif = a - b;
    V mul = a * b;
    bool ok = true;
    for (std::size_t i = 0; i < N; ++i) {
        ok = ok && equiv(sum[i], T(5))
                && equiv(dif[i], T(1))
                && equiv(mul[i], T(6));
    }

    V s = static_cast<T>(7) + a;
    for (std::size_t i = 0; i < N; ++i) ok = ok && equiv(s[i], T(10));
    return ok;
}

template <class T, std::size_t N>
constexpr bool test_basic_arith_int() {
    if (!test_basic_arith<T, N>()) return false;
    using V = cve<T, N>;
    V a = static_cast<T>(7);
    V b = static_cast<T>(2);
    V q = a / b;
    V r = a % b;
    bool ok = true;
    for (std::size_t i = 0; i < N; ++i) ok = ok && q[i] == T(3) && r[i] == T(1);
    return ok;
}

template <class T, std::size_t N>
constexpr bool test_cmp() {
    using V = cve<T, N>;
    using M = cve_mask<T>;
    V a = static_cast<T>(3);
    V b = static_cast<T>(5);

    auto eq  = (a == a);
    auto ne  = (a != b);
    auto lt  = (a <  b);
    auto ge  = (b >= a);
    auto neq = (a == b);
    bool ok = true;
    for (std::size_t i = 0; i < N; ++i) {
        ok = ok && eq[i] == M(-1) && ne[i] == M(-1)
                && lt[i] == M(-1) && ge[i] == M(-1)
                && neq[i] == M(0);
    }
    return ok;
}

template <class T, std::size_t N>
constexpr bool test_bitwise() {
    using V = cve<T, N>;
    V a = static_cast<T>(0b1100);
    V b = static_cast<T>(0b1010);

    V andv = a & b;
    V orv  = a | b;
    V xorv = a ^ b;
    V notv = ~a;
    V shl  = a << T(2);
    V shr  = a >> T(1);
    bool ok = true;
    for (std::size_t i = 0; i < N; ++i) {
        ok = ok && andv[i] == T(0b1000)
                && orv[i]  == T(0b1110)
                && xorv[i] == T(0b0110)
                && notv[i] == T(~T(0b1100))
                && shl[i]  == T(0b1100 << 2)
                && shr[i]  == T(0b1100 >> 1);
    }
    return ok;
}

template <class T, std::size_t N>
constexpr bool run_int() {
    return test_basic_arith_int<T, N>()
        && test_cmp<T, N>()
        && test_bitwise<T, N>();
}

template <class T, std::size_t N>
constexpr bool run_float() {
    if (!test_basic_arith<T, N>()) return false;
    if (!test_cmp<T, N>()) return false;
    using V = cve<T, N>;
    V a = static_cast<T>(10);
    V b = static_cast<T>(4);
    V q = a / b;
    for (std::size_t i = 0; i < N; ++i) if (!equiv(q[i], T(2.5))) return false;
    return true;
}

template <class T>
constexpr bool run_int_all_n() {
    return run_int<T, 2>() && run_int<T, 3>() && run_int<T, 4>()
        && run_int<T, 8>() && run_int<T, 16>();
}
template <class T>
constexpr bool run_float_all_n() {
    return run_float<T, 2>() && run_float<T, 3>() && run_float<T, 4>()
        && run_float<T, 8>() && run_float<T, 16>();
}

static_assert(run_int_all_n<std::int8_t>());
static_assert(run_int_all_n<std::int16_t>());
static_assert(run_int_all_n<std::int32_t>());
static_assert(run_int_all_n<std::int64_t>());
static_assert(run_int_all_n<std::uint8_t>());
static_assert(run_int_all_n<std::uint16_t>());
static_assert(run_int_all_n<std::uint32_t>());
static_assert(run_int_all_n<std::uint64_t>());
static_assert(run_float_all_n<float>());
static_assert(run_float_all_n<double>());

