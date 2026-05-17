#include "../cve.h"
#include "test_util.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <span>

using f4 = cve<float, 4>;
using i8 = cve<std::int16_t, 8>;

static void test_aligned_load() {
    alignas(16) std::array<float, 4> buf = {1, 2, 3, 4};
    f4 v = std::bit_cast<f4>(buf);
    assert(equiv(v[0], 1.0f) && equiv(v[1], 2.0f) && equiv(v[2], 3.0f) && equiv(v[3], 4.0f));
}

static void test_aligned_store() {
    f4 v = {10, 20, 30, 40};
    auto buf = std::bit_cast<std::array<float, 4>>(v);
    assert(equiv(buf[0], 10.0f) && equiv(buf[1], 20.0f) && equiv(buf[2], 30.0f) && equiv(buf[3], 40.0f));
}

static void test_unaligned_load_element_aligned() {
    alignas(16) std::array<float, 8> buf = {0, 1, 2, 3, 4, 5, 6, 7};
    std::array<float, 4> sub;
    std::ranges::copy(std::span{buf}.subspan(1, 4), sub.begin());
    f4 v = std::bit_cast<f4>(sub);
    assert(equiv(v[0], 1.0f) && equiv(v[1], 2.0f) && equiv(v[2], 3.0f) && equiv(v[3], 4.0f));
}

static void test_unaligned_store_element_aligned() {
    f4 v = {100, 200, 300, 400};
    alignas(16) std::array<float, 8> buf = {};
    auto vbuf = std::bit_cast<std::array<float, 4>>(v);
    std::ranges::copy(vbuf, std::span{buf}.subspan(1).begin());
    assert(equiv(buf[0], 0.0f));
    assert(equiv(buf[1], 100.0f) && equiv(buf[2], 200.0f) && equiv(buf[3], 300.0f) && equiv(buf[4], 400.0f));
    assert(equiv(buf[5], 0.0f));
}

static void test_unaligned_load_byte_offset() {
    alignas(16) std::array<std::uint8_t, 64> bytes = {};
    std::array<float, 4> src = {7, 8, 9, 10};
    std::ranges::copy(std::as_bytes(std::span{src}),
                      std::as_writable_bytes(std::span{bytes}).subspan(5).begin());

    std::array<float, 4> dst;
    std::ranges::copy(std::as_bytes(std::span{bytes}).subspan(5, sizeof dst),
                      std::as_writable_bytes(std::span{dst}).begin());
    f4 v = std::bit_cast<f4>(dst);

    assert(equiv(v[0], 7.0f) && equiv(v[1], 8.0f) && equiv(v[2], 9.0f) && equiv(v[3], 10.0f));
}

static void test_unaligned_store_byte_offset() {
    f4 v = {-1, -2, -3, -4};
    alignas(16) std::array<std::uint8_t, 64> bytes = {};
    auto v_bytes = std::bit_cast<std::array<std::byte, sizeof(f4)>>(v);
    std::ranges::copy(v_bytes,
                      std::as_writable_bytes(std::span{bytes}).subspan(3).begin());

    std::array<float, 4> dst;
    std::ranges::copy(std::as_bytes(std::span{bytes}).subspan(3, sizeof dst),
                      std::as_writable_bytes(std::span{dst}).begin());

    assert(equiv(dst[0], -1.0f) && equiv(dst[1], -2.0f) && equiv(dst[2], -3.0f) && equiv(dst[3], -4.0f));
}

static void test_wider_vec_roundtrip() {
    i8 v = {1, 2, 3, 4, 5, 6, 7, 8};
    alignas(16) std::array<std::uint8_t, 32> bytes = {};
    auto v_bytes = std::bit_cast<std::array<std::byte, sizeof(i8)>>(v);
    std::ranges::copy(v_bytes,
                      std::as_writable_bytes(std::span{bytes}).subspan(7).begin());

    std::array<std::int16_t, 8> dst;
    std::ranges::copy(std::as_bytes(std::span{bytes}).subspan(7, sizeof dst),
                      std::as_writable_bytes(std::span{dst}).begin());
    i8 w = std::bit_cast<i8>(dst);

    for (std::size_t i = 0; i < 8; ++i) assert(w[i] == std::int16_t(i + 1));
}

int main() {
    test_aligned_load();
    test_aligned_store();
    test_unaligned_load_element_aligned();
    test_unaligned_store_element_aligned();
    test_unaligned_load_byte_offset();
    test_unaligned_store_byte_offset();
    test_wider_vec_roundtrip();
    return 0;
}
