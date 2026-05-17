#include "cve.h"

#include <cassert>
#include <cstdint>
#include <cstring>

using f4 = cve<float, 4>;
using i8 = cve<std::int16_t, 8>;

static void test_aligned_load() {
    alignas(16) float buf[4] = {1, 2, 3, 4};
    f4 v;
    std::memcpy(&v, buf, sizeof v);
    assert(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);
}

static void test_aligned_store() {
    f4 v = {10, 20, 30, 40};
    alignas(16) float buf[4] = {};
    std::memcpy(buf, &v, sizeof v);
    assert(buf[0] == 10 && buf[1] == 20 && buf[2] == 30 && buf[3] == 40);
}

static void test_unaligned_load_element_aligned() {
    alignas(16) float buf[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    f4 v;
    std::memcpy(&v, buf + 1, sizeof v);
    assert(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);
}

static void test_unaligned_store_element_aligned() {
    f4 v = {100, 200, 300, 400};
    alignas(16) float buf[8] = {};
    std::memcpy(buf + 1, &v, sizeof v);
    assert(buf[0] == 0);
    assert(buf[1] == 100 && buf[2] == 200 && buf[3] == 300 && buf[4] == 400);
    assert(buf[5] == 0);
}

static void test_unaligned_load_byte_offset() {
    alignas(16) std::uint8_t bytes[64] = {};
    const float src[4] = {7, 8, 9, 10};
    std::memcpy(bytes + 5, src, sizeof src);

    f4 v;
    std::memcpy(&v, bytes + 5, sizeof v);
    assert(v[0] == 7 && v[1] == 8 && v[2] == 9 && v[3] == 10);
}

static void test_unaligned_store_byte_offset() {
    f4 v = {-1, -2, -3, -4};
    alignas(16) std::uint8_t bytes[64] = {};
    std::memcpy(bytes + 3, &v, sizeof v);

    float dst[4];
    std::memcpy(dst, bytes + 3, sizeof dst);
    assert(dst[0] == -1 && dst[1] == -2 && dst[2] == -3 && dst[3] == -4);
}

static void test_wider_vec_roundtrip() {
    i8 v = {1, 2, 3, 4, 5, 6, 7, 8};
    alignas(16) std::uint8_t bytes[32] = {};
    std::memcpy(bytes + 7, &v, sizeof v);

    i8 w;
    std::memcpy(&w, bytes + 7, sizeof w);
    for (int i = 0; i < 8; ++i) assert(w[i] == std::int16_t(i + 1));
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
