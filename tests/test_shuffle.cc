#include "../cve.h"
#include "test_util.h"

#include <cassert>
#include <cstdint>

using f4 = cve<float, 4>;
using i4 = cve<std::int32_t, 4>;
using f8 = cve<float, 8>;

static void test_shuffle_single_same_width() {
    f4 v = {10, 20, 30, 40};
    f4 r = cve_shuffle<3, 2, 1, 0>(v);
    assert(equiv(r[0], 40.0f) && equiv(r[1], 30.0f) && equiv(r[2], 20.0f) && equiv(r[3], 10.0f));
}

static void test_shuffle_single_narrowing() {
    f4 v = {10, 20, 30, 40};
    auto r = cve_shuffle<0, 2>(v);
    assert(equiv(r[0], 10.0f) && equiv(r[1], 30.0f));
}

static void test_shuffle_single_widening() {
    cve<float, 2> v = {7, 8};
    auto r = cve_shuffle<0, 1, 0, 1>(v);
    assert(equiv(r[0], 7.0f) && equiv(r[1], 8.0f) && equiv(r[2], 7.0f) && equiv(r[3], 8.0f));
}

static void test_shuffle_two_vec() {
    f4 a = {1, 2, 3, 4};
    f4 b = {10, 20, 30, 40};

    f4 lo = cve_shuffle<0, 1, 4, 5>(a, b);
    assert(equiv(lo[0], 1.0f) && equiv(lo[1], 2.0f) && equiv(lo[2], 10.0f) && equiv(lo[3], 20.0f));

    f4 hi = cve_shuffle<2, 3, 6, 7>(a, b);
    assert(equiv(hi[0], 3.0f) && equiv(hi[1], 4.0f) && equiv(hi[2], 30.0f) && equiv(hi[3], 40.0f));

    f8 cat = cve_shuffle<0, 1, 2, 3, 4, 5, 6, 7>(a, b);
    assert(equiv(cat[0], 1.0f) && equiv(cat[3], 4.0f) && equiv(cat[4], 10.0f) && equiv(cat[7], 40.0f));
}

static void test_convert_float_to_int() {
    f4 v = {1.5f, 2.7f, -3.2f, 4.9f};
    i4 r = cve_convert<std::int32_t>(v);
    assert(r[0] == 1 && r[1] == 2 && r[2] == -3 && r[3] == 4);
}

static void test_convert_int_to_float() {
    i4 v = {1, -2, 3, -4};
    f4 r = cve_convert<float>(v);
    assert(equiv(r[0], 1.0f) && equiv(r[1], -2.0f) && equiv(r[2], 3.0f) && equiv(r[3], -4.0f));
}

static void test_convert_narrowing() {
    cve<std::int64_t, 4> v = {1, 2, 3, 4};
    cve<std::int16_t, 4> r = cve_convert<std::int16_t>(v);
    assert(r[0] == 1 && r[3] == 4);
}

int main() {
    test_shuffle_single_same_width();
    test_shuffle_single_narrowing();
    test_shuffle_single_widening();
    test_shuffle_two_vec();
    test_convert_float_to_int();
    test_convert_int_to_float();
    test_convert_narrowing();
    return 0;
}
