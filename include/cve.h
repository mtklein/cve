// cve.h — C++ polyfill for clang's ext_vector_type.
//
// Usage:
//   using f4 = cve<float, 4>;
//   f4 a = 1.0f;             // splat
//   f4 b = {1, 2, 3, 4};     // per-element
//   f4 c = a + b * 2.0f;     // scalar broadcast
//   float x = c[0];          // index
//   c.xy = {7, 8};           // swizzle write (lvalue)
//   f4 d = c.wzyx;           // swizzle read (rvalue)
//
// Backends (auto-selected):
//   __clang__   -> native ext_vector_type
//   __GNUC__    -> struct wrapping a __attribute__((vector_size)) member
//   else        -> struct wrapping an aligned array, looped ops
// Force the portable backend with -DCVE_FORCE_PORTABLE.
//
// Supported (T, N): T in {int8..int64, uint8..uint64, float, double},
//                   N in {2, 4, 8, 16}.
// Swizzles (xyzw + rgba aliases) are provided for N in {2, 4}.

#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

#if defined(CVE_FORCE_PORTABLE)
  #define CVE_BACKEND_PORTABLE 1
  #define CVE_BACKEND_NAME "portable"
#elif defined(__clang__)
  #define CVE_BACKEND_CLANG 1
  #define CVE_BACKEND_NAME "clang"
#elif defined(__GNUC__)
  #define CVE_BACKEND_GCC 1
  #define CVE_BACKEND_NAME "gcc"
#else
  #define CVE_BACKEND_PORTABLE 1
  #define CVE_BACKEND_NAME "portable"
#endif

namespace cve_impl {

// ---------- comparison-result mask type -------------------------------------
// Comparisons on a vec<T,N> return a vec<mask_t<T>,N> whose lanes are
// 0 (false) or -1 (true), matching clang and GCC's native vector compares.
// We use the fundamental integer types (char/short/int/long long) because
// that's what clang's ext_vector_type compare returns — `int8_t` (typically
// `signed char`) is a *distinct* type from `char` on most platforms, and
// using `int8_t` here would fail the clang-native build.
// Clang picks the shortest fundamental signed integer of the target size, so
// on LP64 (macOS, Linux) it picks `long` for 8 bytes and on LLP64 (Windows)
// it picks `long long`. Mirror that here.
template <class T>
using mask_t =
    std::conditional_t<sizeof(T) == 1, char,
    std::conditional_t<sizeof(T) == 2, short,
    std::conditional_t<sizeof(T) == 4, int,
    std::conditional_t<sizeof(T) == sizeof(long), long, long long>>>>;

// ============================================================================
// Clang backend: a thin alias around ext_vector_type. No wrapper needed —
// clang's native vector type already provides splat, brace init, ops, swizzle.
// ============================================================================
#if defined(CVE_BACKEND_CLANG)

template <class T, int N>
struct native { typedef T type __attribute__((ext_vector_type(N))); };

#else
// ============================================================================
// Wrapper backends (GCC and portable). The wrapper:
//   * holds storage (vector_size for GCC; aligned array for portable)
//   * provides scalar splat / per-element constructors
//   * overlays an anonymous union of named members + swizzle proxies
// ============================================================================

template <class T, int N> struct vec;

// ---------- storage ---------------------------------------------------------
#if defined(CVE_BACKEND_GCC)

// GCC's vector_size is the storage. Indexing and arithmetic are built-in.
template <class T, int N>
struct storage_holder {
    typedef T type __attribute__((vector_size(N * sizeof(T))));
};
template <class T, int N>
using storage_t = typename storage_holder<T, N>::type;

#else // CVE_BACKEND_PORTABLE

// Plain aligned array with operator[] so the rest of the code is uniform.
template <class T, int N>
struct alignas(N * sizeof(T)) storage_t {
    T e[N];
    constexpr T&       operator[](int i)       { return e[i]; }
    constexpr const T& operator[](int i) const { return e[i]; }
};

#endif

// ---------- swizzle proxy ---------------------------------------------------
// Layout-compatible with vec<T,N>; placed in an anonymous union with the
// parent's storage so writes through the proxy mutate the parent.
template <class T, int N, int... Is>
struct swizzle_proxy {
    storage_t<T, N> data;

    // A user-declared default ctor (defaulted but declared) disqualifies us
    // from being an aggregate. Without this, `v.xy = {1, 2}` is ambiguous
    // because the brace-init-list could either aggregate-init swizzle_proxy
    // or construct vec<T,2>.
    swizzle_proxy() = default;

    static constexpr int K = (int)sizeof...(Is);

    // rvalue: gather the selected lanes into a vec<T, K>.
    constexpr operator vec<T, K>() const {
        return vec<T, K>{ data[Is]... };
    }

    // lvalue: scatter from a vec<T, K> back into the source lanes.
    constexpr swizzle_proxy& operator=(const vec<T, K>& rhs) {
        int idx[K] = { Is... };
        for (int i = 0; i < K; ++i) data[idx[i]] = rhs[i];
        return *this;
    }

    // lvalue: splat a scalar across the selected lanes.
    constexpr swizzle_proxy& operator=(T s) {
        for (int i : {Is...}) data[i] = s;
        return *this;
    }

    // lvalue: cross-assign from another proxy of matching width.
    template <int M, int... Js>
        requires (sizeof...(Js) == K)
    constexpr swizzle_proxy& operator=(const swizzle_proxy<T, M, Js...>& rhs) {
        return *this = (vec<T, K>)rhs;
    }
};

// ---------- common vec members (constructors, indexing) ---------------------
// CVE_VEC_COMMON expects an identifier 'N' to be in scope inside the struct:
//   - in the primary template, the template parameter `int N` plays that role
//   - in specializations (vec<T,2>, vec<T,4>) declare `static constexpr int N`
//     before invoking the macro.
#define CVE_VEC_COMMON                                                        \
    vec() = default;                                                          \
                                                                              \
    /* splat: cve<float,4> v = 1.0f; */                                       \
    constexpr vec(T s) {                                                      \
        for (int i = 0; i < N; ++i) v[i] = s;                                 \
    }                                                                         \
                                                                              \
    /* per-element: cve<float,4>{1,2,3,4}; requires exactly N args */         \
    template <class... Args>                                                  \
        requires (sizeof...(Args) == N) && (N != 1)                           \
              && ((std::is_convertible_v<Args, T>) && ...)                    \
    constexpr vec(Args... args) {                                             \
        T tmp[N] = { static_cast<T>(args)... };                               \
        for (int i = 0; i < N; ++i) v[i] = tmp[i];                            \
    }                                                                         \
                                                                              \
    constexpr T&       operator[](int i)       { return v[i]; }               \
    constexpr const T& operator[](int i) const { return v[i]; }

// ---------- hidden friends: operators on vec --------------------------------
// Non-template friends are findable via ADL on a vec argument and allow
// implicit conversions, so they handle:
//   vec ⊗ vec, vec ⊗ T, T ⊗ vec, proxy ⊗ vec, vec ⊗ proxy, etc.
#define CVE_FRIEND_BINOP(OP)                                                  \
    friend constexpr vec operator OP(vec a, vec b) {                          \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a.v[i] OP b.v[i];                \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(vec a, T b) {                            \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a.v[i] OP b;                     \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(T a, vec b) {                            \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a OP b.v[i];                     \
        return r;                                                             \
    }

// Comparison: returns a vec<mask_t<T>, N> where each lane is 0 or -1.
#define CVE_FRIEND_CMP(OP)                                                    \
    friend constexpr vec<mask_t<T>, N> operator OP(vec a, vec b) {            \
        vec<mask_t<T>, N> r;                                                  \
        for (int i = 0; i < N; ++i)                                           \
            r.v[i] = (a.v[i] OP b.v[i]) ? mask_t<T>(-1) : mask_t<T>(0);       \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec<mask_t<T>, N> operator OP(vec a, T b) {              \
        vec<mask_t<T>, N> r;                                                  \
        for (int i = 0; i < N; ++i)                                           \
            r.v[i] = (a.v[i] OP b) ? mask_t<T>(-1) : mask_t<T>(0);            \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec<mask_t<T>, N> operator OP(T a, vec b) {              \
        vec<mask_t<T>, N> r;                                                  \
        for (int i = 0; i < N; ++i)                                           \
            r.v[i] = (a OP b.v[i]) ? mask_t<T>(-1) : mask_t<T>(0);            \
        return r;                                                             \
    }

// Bitwise binary op — constrained to integer T.
#define CVE_FRIEND_BITOP(OP)                                                  \
    friend constexpr vec operator OP(vec a, vec b)                            \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a.v[i] OP b.v[i];                \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(vec a, T b)                              \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a.v[i] OP b;                     \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(T a, vec b)                              \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = a OP b.v[i];                     \
        return r;                                                             \
    }

#define CVE_FRIEND_OPS                                                        \
    CVE_FRIEND_BINOP(+)                                                       \
    CVE_FRIEND_BINOP(-)                                                       \
    CVE_FRIEND_BINOP(*)                                                       \
    CVE_FRIEND_BINOP(/)                                                       \
    CVE_FRIEND_CMP(==)                                                        \
    CVE_FRIEND_CMP(!=)                                                        \
    CVE_FRIEND_CMP(<)                                                         \
    CVE_FRIEND_CMP(<=)                                                        \
    CVE_FRIEND_CMP(>)                                                         \
    CVE_FRIEND_CMP(>=)                                                        \
    CVE_FRIEND_BITOP(&)                                                       \
    CVE_FRIEND_BITOP(|)                                                       \
    CVE_FRIEND_BITOP(^)                                                       \
    CVE_FRIEND_BITOP(%)                                                       \
    CVE_FRIEND_BITOP(<<)                                                      \
    CVE_FRIEND_BITOP(>>)                                                      \
    friend constexpr vec operator-(vec a) {                                   \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = -a.v[i];                         \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator+(vec a) { return a; }                       \
    friend constexpr vec operator~(vec a)                                     \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (int i = 0; i < N; ++i) r.v[i] = ~a.v[i];                         \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec& operator+=(vec& a, vec b) { return a = a + b; }     \
    friend constexpr vec& operator-=(vec& a, vec b) { return a = a - b; }     \
    friend constexpr vec& operator*=(vec& a, vec b) { return a = a * b; }     \
    friend constexpr vec& operator/=(vec& a, vec b) { return a = a / b; }     \
    friend constexpr vec& operator+=(vec& a, T b)   { return a = a + b; }     \
    friend constexpr vec& operator-=(vec& a, T b)   { return a = a - b; }     \
    friend constexpr vec& operator*=(vec& a, T b)   { return a = a * b; }     \
    friend constexpr vec& operator/=(vec& a, T b)   { return a = a / b; }

// Anonymous-struct extension warnings (we rely on it for .x/.y/.z/.w).
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
  #pragma clang diagnostic ignored "-Wnested-anon-types"
#elif defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpedantic"
#endif

// ============================================================================
// Primary template — N with no swizzle support (8, 16, etc).
// ============================================================================
template <class T, int N>
struct vec {
    storage_t<T, N> v;
    CVE_VEC_COMMON
    CVE_FRIEND_OPS
};

// ============================================================================
// vec<T, 2> — has .x/.y, .r/.g, and 2-component swizzles.
// ============================================================================
#define CVE_S2_FROM_2(L, A) \
    swizzle_proxy<T, 2, A, 0> L##x; swizzle_proxy<T, 2, A, 1> L##y;
#define CVE_S2_ALL_2 \
    CVE_S2_FROM_2(x, 0) CVE_S2_FROM_2(y, 1)

#define CVE_S2_FROM_2_RGBA(L, A) \
    swizzle_proxy<T, 2, A, 0> L##r; swizzle_proxy<T, 2, A, 1> L##g;
#define CVE_S2_ALL_2_RGBA \
    CVE_S2_FROM_2_RGBA(r, 0) CVE_S2_FROM_2_RGBA(g, 1)

template <class T>
struct vec<T, 2> {
    static constexpr int N = 2;
    union {
        storage_t<T, 2> v;
        struct { T x, y; };
        struct { T r, g; };
        CVE_S2_ALL_2
        CVE_S2_ALL_2_RGBA
    };
    CVE_VEC_COMMON
    CVE_FRIEND_OPS
};

// ============================================================================
// vec<T, 4> — full xyzw / rgba named members, 2- and 4-component swizzles.
// (3-component swizzles are intentionally omitted because vec<T,3> is not in
// the supported type matrix.)
// ============================================================================

// 2-component swizzles on vec<T,4>
#define CVE_S4_FROM_2(L, A) \
    swizzle_proxy<T, 4, A, 0> L##x; swizzle_proxy<T, 4, A, 1> L##y; \
    swizzle_proxy<T, 4, A, 2> L##z; swizzle_proxy<T, 4, A, 3> L##w;
#define CVE_S4_ALL_2 \
    CVE_S4_FROM_2(x, 0) CVE_S4_FROM_2(y, 1) \
    CVE_S4_FROM_2(z, 2) CVE_S4_FROM_2(w, 3)

#define CVE_S4_FROM_2_RGBA(L, A) \
    swizzle_proxy<T, 4, A, 0> L##r; swizzle_proxy<T, 4, A, 1> L##g; \
    swizzle_proxy<T, 4, A, 2> L##b; swizzle_proxy<T, 4, A, 3> L##a;
#define CVE_S4_ALL_2_RGBA \
    CVE_S4_FROM_2_RGBA(r, 0) CVE_S4_FROM_2_RGBA(g, 1) \
    CVE_S4_FROM_2_RGBA(b, 2) CVE_S4_FROM_2_RGBA(a, 3)

// 4-component swizzles on vec<T,4>: 4^4 = 256 names per alias set.
#define CVE_S4_FROM_4(NAME, A, B, C, D) \
    swizzle_proxy<T, 4, A, B, C, D> NAME;

#define CVE_S4_4_L4(L, A, B, C) \
    CVE_S4_FROM_4(L##x, A, B, C, 0) CVE_S4_FROM_4(L##y, A, B, C, 1) \
    CVE_S4_FROM_4(L##z, A, B, C, 2) CVE_S4_FROM_4(L##w, A, B, C, 3)
#define CVE_S4_4_L3(L, A, B) \
    CVE_S4_4_L4(L##x, A, B, 0) CVE_S4_4_L4(L##y, A, B, 1) \
    CVE_S4_4_L4(L##z, A, B, 2) CVE_S4_4_L4(L##w, A, B, 3)
#define CVE_S4_4_L2(L, A) \
    CVE_S4_4_L3(L##x, A, 0) CVE_S4_4_L3(L##y, A, 1) \
    CVE_S4_4_L3(L##z, A, 2) CVE_S4_4_L3(L##w, A, 3)
#define CVE_S4_ALL_4 \
    CVE_S4_4_L2(x, 0) CVE_S4_4_L2(y, 1) \
    CVE_S4_4_L2(z, 2) CVE_S4_4_L2(w, 3)

#define CVE_S4_4_L4_RGBA(L, A, B, C) \
    CVE_S4_FROM_4(L##r, A, B, C, 0) CVE_S4_FROM_4(L##g, A, B, C, 1) \
    CVE_S4_FROM_4(L##b, A, B, C, 2) CVE_S4_FROM_4(L##a, A, B, C, 3)
#define CVE_S4_4_L3_RGBA(L, A, B) \
    CVE_S4_4_L4_RGBA(L##r, A, B, 0) CVE_S4_4_L4_RGBA(L##g, A, B, 1) \
    CVE_S4_4_L4_RGBA(L##b, A, B, 2) CVE_S4_4_L4_RGBA(L##a, A, B, 3)
#define CVE_S4_4_L2_RGBA(L, A) \
    CVE_S4_4_L3_RGBA(L##r, A, 0) CVE_S4_4_L3_RGBA(L##g, A, 1) \
    CVE_S4_4_L3_RGBA(L##b, A, 2) CVE_S4_4_L3_RGBA(L##a, A, 3)
#define CVE_S4_ALL_4_RGBA \
    CVE_S4_4_L2_RGBA(r, 0) CVE_S4_4_L2_RGBA(g, 1) \
    CVE_S4_4_L2_RGBA(b, 2) CVE_S4_4_L2_RGBA(a, 3)

template <class T>
struct vec<T, 4> {
    static constexpr int N = 4;
    union {
        storage_t<T, 4> v;
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        CVE_S4_ALL_2
        CVE_S4_ALL_2_RGBA
        CVE_S4_ALL_4
        CVE_S4_ALL_4_RGBA
    };
    CVE_VEC_COMMON
    CVE_FRIEND_OPS
};

#if defined(__clang__)
  #pragma clang diagnostic pop
#elif defined(__GNUC__)
  #pragma GCC diagnostic pop
#endif

// ---------- proxy ⊗ proxy / proxy ⊗ scalar operators ------------------------
// (proxy ⊗ vec is already handled by vec's hidden friends + implicit conv.)
#define CVE_PROXY_BINOP(OP)                                                   \
    template <class T, int N1, int... Is, int N2, int... Js>                  \
        requires(sizeof...(Is) == sizeof...(Js))                              \
    constexpr auto operator OP(swizzle_proxy<T, N1, Is...> a,                 \
                               swizzle_proxy<T, N2, Js...> b)                 \
        -> vec<T, sizeof...(Is)>                                              \
    {                                                                         \
        constexpr int K = (int)sizeof...(Is);                                 \
        return (vec<T, K>)a OP (vec<T, K>)b;                                  \
    }                                                                         \
    template <class T, int N, int... Is>                                      \
    constexpr auto operator OP(swizzle_proxy<T, N, Is...> a, T b)             \
        -> vec<T, sizeof...(Is)>                                              \
    { return (vec<T, sizeof...(Is)>)a OP b; }                                 \
    template <class T, int N, int... Is>                                      \
    constexpr auto operator OP(T a, swizzle_proxy<T, N, Is...> b)             \
        -> vec<T, sizeof...(Is)>                                              \
    { return a OP (vec<T, sizeof...(Is)>)b; }

CVE_PROXY_BINOP(+)
CVE_PROXY_BINOP(-)
CVE_PROXY_BINOP(*)
CVE_PROXY_BINOP(/)
#undef CVE_PROXY_BINOP

#endif // !CVE_BACKEND_CLANG

} // namespace cve_impl

// ============================================================================
// Public type alias.
// ============================================================================
#if defined(CVE_BACKEND_CLANG)
template <class T, int N>
using cve = typename cve_impl::native<T, N>::type;
#else
template <class T, int N>
using cve = cve_impl::vec<T, N>;
#endif

// Mask vec type produced by a comparison on cve<T, N>.
template <class T>
using cve_mask = cve_impl::mask_t<T>;

// ============================================================================
// Free-function helpers: cve_shuffle and cve_convert.
// Mirror __builtin_shufflevector and __builtin_convertvector.
// ============================================================================
namespace cve_impl {

// Extract (T, N) from any vec-like type (works for both ext_vector_type and
// our wrapper struct because both support sizeof and operator[]).
template <class V>
struct vec_traits {
    using element_type =
        std::remove_reference_t<std::remove_cv_t<decltype(std::declval<V&>()[0])>>;
    static constexpr int length =
        (int)(sizeof(V) / sizeof(element_type));
};

#if !defined(CVE_BACKEND_CLANG)
// Helper: pick from one of two vecs by compile-time index (I < N -> a, else b).
template <int I, class V>
constexpr auto pick(V a, V b) {
    constexpr int N = vec_traits<V>::length;
    if constexpr (I < N) return a[I];
    else                 return b[I - N];
}
#endif

} // namespace cve_impl

// Single-vec shuffle: cve_shuffle<2,3,0,1>(v).
template <int... Is, class V>
constexpr auto cve_shuffle(V v) {
#if defined(CVE_BACKEND_CLANG)
    return __builtin_shufflevector(v, v, Is...);
#else
    using T = typename cve_impl::vec_traits<V>::element_type;
    return cve<T, sizeof...(Is)>{ v[Is]... };
#endif
}

// Two-vec shuffle: indices in [0,N) pick from a, [N,2N) pick from b.
template <int... Is, class V>
constexpr auto cve_shuffle(V a, V b) {
#if defined(CVE_BACKEND_CLANG)
    return __builtin_shufflevector(a, b, Is...);
#else
    using T = typename cve_impl::vec_traits<V>::element_type;
    return cve<T, sizeof...(Is)>{ cve_impl::pick<Is, V>(a, b)... };
#endif
}

// Element-type conversion: cve_convert<int>(float_vec) -> int_vec of same N.
template <class To, class V>
constexpr auto cve_convert(V v) {
    constexpr int N = cve_impl::vec_traits<V>::length;
#if defined(CVE_BACKEND_CLANG)
    typedef To result_t __attribute__((ext_vector_type(N)));
    return __builtin_convertvector(v, result_t);
#else
    return [&]<int... Is>(std::integer_sequence<int, Is...>) {
        return cve<To, N>{ static_cast<To>(v[Is])... };
    }(std::make_integer_sequence<int, N>{});
#endif
}
