#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace cve_impl {

template <class To>
constexpr To implicit_cast(std::type_identity_t<To> x) { return x; }

// Clang's ext_vector_type compare returns the shortest fundamental signed
// integer of the operand's size — `char` for 1 byte (NOT `signed char`, which
// is a distinct type), `long` for 8 bytes on LP64 (macOS, Linux) but
// `long long` on LLP64 (Windows). Using <cstdint> aliases here would mismatch
// clang's result type and break the clang-native build.
template <class T>
using mask_t =
    std::conditional_t<sizeof(T) == 1, char,
    std::conditional_t<sizeof(T) == 2, short,
    std::conditional_t<sizeof(T) == 4, int,
    std::conditional_t<sizeof(T) == sizeof(long), long, long long>>>>;

#if defined(__clang__) && !defined(CVE_FORCE_PORTABLE)

template <class T, std::size_t N>
struct native { typedef T type __attribute__((ext_vector_type(N))); };

#else

template <class T, std::size_t N> struct vec;

template <class T, std::size_t N>
struct storage_t {
#if defined(__GNUC__) && !defined(CVE_FORCE_PORTABLE)
    typedef T native __attribute__((vector_size(N * sizeof(T))));
    native e;
#else
    alignas(N * sizeof(T)) std::array<T, N> e;
#endif
    constexpr T&       operator[](std::size_t i)       { return e[i]; }
    constexpr const T& operator[](std::size_t i) const { return e[i]; }
};

template <class T, std::size_t N, std::size_t... Is>
struct swizzle_proxy {
    storage_t<T, N> data;

    // A user-declared default ctor disqualifies us from being an aggregate.
    // Without this, `v.xy = {1, 2}` is ambiguous: the brace-init-list could
    // either aggregate-init swizzle_proxy or construct vec<T,2>.
    swizzle_proxy() = default;

    static constexpr std::size_t K = sizeof...(Is);

    constexpr operator vec<T, K>() const {
        return vec<T, K>{ data[Is]... };
    }

    constexpr swizzle_proxy& operator=(const vec<T, K>& rhs) {
        std::size_t j = 0;
        ((data[Is] = rhs[j++]), ...);
        return *this;
    }

    constexpr swizzle_proxy& operator=(T s) {
        ((data[Is] = s), ...);
        return *this;
    }

    template <std::size_t M, std::size_t... Js>
        requires (sizeof...(Js) == K)
    constexpr swizzle_proxy& operator=(const swizzle_proxy<T, M, Js...>& rhs) {
        return *this = implicit_cast<vec<T, K>>(rhs);
    }
};

// Expects an identifier `N` to be in scope: the template parameter in the
// primary vec template, or a `static constexpr std::size_t N = ...` declared
// above the macro invocation in a specialization.
#define CVE_VEC_COMMON                                                        \
    vec() = default;                                                          \
                                                                              \
    constexpr vec(T s) {                                                      \
        for (std::size_t i = 0; i < N; ++i) v[i] = s;                         \
    }                                                                         \
                                                                              \
    template <class... Args>                                                  \
        requires (sizeof...(Args) == N) && (N != 1)                           \
              && ((std::is_convertible_v<Args, T>) && ...)                    \
    constexpr vec(Args... args) {                                             \
        std::size_t j = 0;                                                    \
        ((v[j++] = static_cast<T>(args)), ...);                               \
    }                                                                         \
                                                                              \
    constexpr T&       operator[](std::size_t i)       { return v[i]; }       \
    constexpr const T& operator[](std::size_t i) const { return v[i]; }

// These are friends, not free function templates, so ADL on a vec argument
// finds them and lets implicit conversions kick in for the other argument.
// That covers proxy ⊗ vec, vec ⊗ proxy, scalar ⊗ vec, etc. — template arg
// deduction wouldn't consider those conversions on a free template.
#define CVE_FRIEND_BINOP(OP)                                                  \
    friend constexpr vec operator OP(vec a, vec b) {                          \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a.v[i] OP b.v[i]);                        \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(vec a, T b) {                            \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a.v[i] OP b);                             \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(T a, vec b) {                            \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a OP b.v[i]);                             \
        return r;                                                             \
    }

// TODO: cve_select(mask, a, b) — lane-wise pick from a or b based on
// mask sign bit. Natural pairing with the comparison ops above.
#define CVE_FRIEND_CMP(OP)                                                    \
    friend constexpr vec<mask_t<T>, N> operator OP(vec a, vec b) {            \
        vec<mask_t<T>, N> r;                                                  \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = (a.v[i] OP b.v[i]) ? mask_t<T>(-1) : mask_t<T>(0);       \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec<mask_t<T>, N> operator OP(vec a, T b) {              \
        vec<mask_t<T>, N> r;                                                  \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = (a.v[i] OP b) ? mask_t<T>(-1) : mask_t<T>(0);            \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec<mask_t<T>, N> operator OP(T a, vec b) {              \
        vec<mask_t<T>, N> r;                                                  \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = (a OP b.v[i]) ? mask_t<T>(-1) : mask_t<T>(0);            \
        return r;                                                             \
    }

#define CVE_FRIEND_BITOP(OP)                                                  \
    friend constexpr vec operator OP(vec a, vec b)                            \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a.v[i] OP b.v[i]);                        \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(vec a, T b)                              \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a.v[i] OP b);                             \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator OP(T a, vec b)                              \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(a OP b.v[i]);                             \
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
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(-a.v[i]);                                 \
        return r;                                                             \
    }                                                                         \
    friend constexpr vec operator+(vec a) { return a; }                       \
    friend constexpr vec operator~(vec a)                                     \
        requires std::is_integral_v<T> {                                      \
        vec r;                                                                \
        for (std::size_t i = 0; i < N; ++i)                                   \
            r.v[i] = static_cast<T>(~a.v[i]);                                 \
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

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
  #pragma clang diagnostic ignored "-Wnested-anon-types"
  #pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wpedantic"
  #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

template <class T, std::size_t N>
struct vec {
    storage_t<T, N> v;
    CVE_VEC_COMMON
    CVE_FRIEND_OPS
};

#define CVE_S2_FROM_2(L, A)                                                   \
    swizzle_proxy<T, 2, A, 0> L##x; swizzle_proxy<T, 2, A, 1> L##y;
#define CVE_S2_ALL_2                                                          \
    CVE_S2_FROM_2(x, 0) CVE_S2_FROM_2(y, 1)

#define CVE_S2_FROM_2_RGBA(L, A)                                              \
    swizzle_proxy<T, 2, A, 0> L##r; swizzle_proxy<T, 2, A, 1> L##g;
#define CVE_S2_ALL_2_RGBA                                                     \
    CVE_S2_FROM_2_RGBA(r, 0) CVE_S2_FROM_2_RGBA(g, 1)

template <class T>
struct vec<T, 2> {
    static constexpr std::size_t N = 2;
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

// 3-component swizzles (.xyz, .rgb) are intentionally omitted: they'd return
// vec<T,3>, and N=3 isn't in the supported type matrix (GCC's vector_size
// rejects non-power-of-2 widths, and exposing N=3 would add a separate
// padding-aware storage path we don't otherwise need).
#define CVE_S4_FROM_2(L, A)                                                   \
    swizzle_proxy<T, 4, A, 0> L##x; swizzle_proxy<T, 4, A, 1> L##y;           \
    swizzle_proxy<T, 4, A, 2> L##z; swizzle_proxy<T, 4, A, 3> L##w;
#define CVE_S4_ALL_2                                                          \
    CVE_S4_FROM_2(x, 0) CVE_S4_FROM_2(y, 1)                                   \
    CVE_S4_FROM_2(z, 2) CVE_S4_FROM_2(w, 3)

#define CVE_S4_FROM_2_RGBA(L, A)                                              \
    swizzle_proxy<T, 4, A, 0> L##r; swizzle_proxy<T, 4, A, 1> L##g;           \
    swizzle_proxy<T, 4, A, 2> L##b; swizzle_proxy<T, 4, A, 3> L##a;
#define CVE_S4_ALL_2_RGBA                                                     \
    CVE_S4_FROM_2_RGBA(r, 0) CVE_S4_FROM_2_RGBA(g, 1)                         \
    CVE_S4_FROM_2_RGBA(b, 2) CVE_S4_FROM_2_RGBA(a, 3)

#define CVE_S4_FROM_4(NAME, A, B, C, D)                                       \
    swizzle_proxy<T, 4, A, B, C, D> NAME;

#define CVE_S4_4_L4(L, A, B, C)                                               \
    CVE_S4_FROM_4(L##x, A, B, C, 0) CVE_S4_FROM_4(L##y, A, B, C, 1)           \
    CVE_S4_FROM_4(L##z, A, B, C, 2) CVE_S4_FROM_4(L##w, A, B, C, 3)
#define CVE_S4_4_L3(L, A, B)                                                  \
    CVE_S4_4_L4(L##x, A, B, 0) CVE_S4_4_L4(L##y, A, B, 1)                     \
    CVE_S4_4_L4(L##z, A, B, 2) CVE_S4_4_L4(L##w, A, B, 3)
#define CVE_S4_4_L2(L, A)                                                     \
    CVE_S4_4_L3(L##x, A, 0) CVE_S4_4_L3(L##y, A, 1)                           \
    CVE_S4_4_L3(L##z, A, 2) CVE_S4_4_L3(L##w, A, 3)
#define CVE_S4_ALL_4                                                          \
    CVE_S4_4_L2(x, 0) CVE_S4_4_L2(y, 1)                                       \
    CVE_S4_4_L2(z, 2) CVE_S4_4_L2(w, 3)

#define CVE_S4_4_L4_RGBA(L, A, B, C)                                          \
    CVE_S4_FROM_4(L##r, A, B, C, 0) CVE_S4_FROM_4(L##g, A, B, C, 1)           \
    CVE_S4_FROM_4(L##b, A, B, C, 2) CVE_S4_FROM_4(L##a, A, B, C, 3)
#define CVE_S4_4_L3_RGBA(L, A, B)                                             \
    CVE_S4_4_L4_RGBA(L##r, A, B, 0) CVE_S4_4_L4_RGBA(L##g, A, B, 1)           \
    CVE_S4_4_L4_RGBA(L##b, A, B, 2) CVE_S4_4_L4_RGBA(L##a, A, B, 3)
#define CVE_S4_4_L2_RGBA(L, A)                                                \
    CVE_S4_4_L3_RGBA(L##r, A, 0) CVE_S4_4_L3_RGBA(L##g, A, 1)                 \
    CVE_S4_4_L3_RGBA(L##b, A, 2) CVE_S4_4_L3_RGBA(L##a, A, 3)
#define CVE_S4_ALL_4_RGBA                                                     \
    CVE_S4_4_L2_RGBA(r, 0) CVE_S4_4_L2_RGBA(g, 1)                             \
    CVE_S4_4_L2_RGBA(b, 2) CVE_S4_4_L2_RGBA(a, 3)

template <class T>
struct vec<T, 4> {
    static constexpr std::size_t N = 4;
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

// proxy ⊗ vec / vec ⊗ proxy are resolved through vec's hidden friends + the
// proxy-to-vec conversion; only the remaining cases need free templates.
#define CVE_PROXY_BINOP(OP)                                                   \
    template <class T,                                                        \
              std::size_t N1, std::size_t... Is,                              \
              std::size_t N2, std::size_t... Js>                              \
        requires(sizeof...(Is) == sizeof...(Js))                              \
    constexpr auto operator OP(swizzle_proxy<T, N1, Is...> a,                 \
                               swizzle_proxy<T, N2, Js...> b)                 \
        -> vec<T, sizeof...(Is)>                                              \
    {                                                                         \
        constexpr std::size_t K = sizeof...(Is);                              \
        return implicit_cast<vec<T, K>>(a) OP implicit_cast<vec<T, K>>(b);    \
    }                                                                         \
    template <class T, std::size_t N, std::size_t... Is>                      \
    constexpr auto operator OP(swizzle_proxy<T, N, Is...> a, T b)             \
        -> vec<T, sizeof...(Is)>                                              \
    { return implicit_cast<vec<T, sizeof...(Is)>>(a) OP b; }                  \
    template <class T, std::size_t N, std::size_t... Is>                      \
    constexpr auto operator OP(T a, swizzle_proxy<T, N, Is...> b)             \
        -> vec<T, sizeof...(Is)>                                              \
    { return a OP implicit_cast<vec<T, sizeof...(Is)>>(b); }

CVE_PROXY_BINOP(+)
CVE_PROXY_BINOP(-)
CVE_PROXY_BINOP(*)
CVE_PROXY_BINOP(/)
#undef CVE_PROXY_BINOP

#endif // !__clang__

} // namespace cve_impl

#if defined(__clang__) && !defined(CVE_FORCE_PORTABLE)
template <class T, std::size_t N>
using cve = typename cve_impl::native<T, N>::type;
#else
template <class T, std::size_t N>
using cve = cve_impl::vec<T, N>;
#endif

template <class T>
using cve_mask = cve_impl::mask_t<T>;

namespace cve_impl {

template <class V>
struct vec_traits {
    using element_type =
        std::remove_reference_t<std::remove_cv_t<decltype(std::declval<V&>()[0])>>;
    static constexpr std::size_t length =
        sizeof(V) / sizeof(element_type);
};

#if !defined(__clang__) || defined(CVE_FORCE_PORTABLE)
template <std::size_t I, class V>
constexpr auto pick(V a, V b) {
    constexpr std::size_t N = vec_traits<V>::length;
    if constexpr (I < N) return a[I];
    else                 return b[I - N];
}
#endif

} // namespace cve_impl

template <std::size_t... Is, class V>
constexpr auto cve_shuffle(V v) {
#if defined(__clang__) && !defined(CVE_FORCE_PORTABLE)
    return __builtin_shufflevector(v, v, Is...);
#else
    using T = typename cve_impl::vec_traits<V>::element_type;
    return cve<T, sizeof...(Is)>{ v[Is]... };
#endif
}

template <std::size_t... Is, class V>
constexpr auto cve_shuffle(V a, V b) {
#if defined(__clang__) && !defined(CVE_FORCE_PORTABLE)
    return __builtin_shufflevector(a, b, Is...);
#else
    using T = typename cve_impl::vec_traits<V>::element_type;
    return cve<T, sizeof...(Is)>{ cve_impl::pick<Is, V>(a, b)... };
#endif
}

// TODO: math builtins — cve_min, cve_max, cve_abs, cve_sqrt, cve_floor,
// cve_ceil, cve_round, cve_fma. Clang has __builtin_elementwise_* for these;
// GCC has nothing analogous, so the wrapper backends would loop with the
// scalar form from <cmath>.
template <class To, class V>
constexpr auto cve_convert(V v) {
    constexpr std::size_t N = cve_impl::vec_traits<V>::length;
#if defined(__clang__) && !defined(CVE_FORCE_PORTABLE)
    typedef To result_t __attribute__((ext_vector_type(N)));
    return __builtin_convertvector(v, result_t);
#else
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return cve<To, N>{ static_cast<To>(v[Is])... };
    }(std::make_index_sequence<N>{});
#endif
}
