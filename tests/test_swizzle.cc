#include "cve.h"
#include "test_util.h"

#include <cassert>

using f2 = cve<float, 2>;
using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

static void test_named_read() {
    f4 v = {10, 20, 30, 40};
    assert(equiv(v.x, 10.0f) && equiv(v.y, 20.0f) && equiv(v.z, 30.0f) && equiv(v.w, 40.0f));
    assert(equiv(v.r, 10.0f) && equiv(v.g, 20.0f) && equiv(v.b, 30.0f) && equiv(v.a, 40.0f));

    f2 p = {7, 8};
    assert(equiv(p.x, 7.0f) && equiv(p.y, 8.0f));
    assert(equiv(p.r, 7.0f) && equiv(p.g, 8.0f));
}

static void test_named_write() {
    f4 v = 0.0f;
    v.x = 1; v.y = 2; v.z = 3; v.w = 4;
    assert(equiv(v[0], 1.0f) && equiv(v[1], 2.0f) && equiv(v[2], 3.0f) && equiv(v[3], 4.0f));

    v.r = 9;
    assert(equiv(v[0], 9.0f));
}

static void test_swizzle_read_2() {
    f4 v = {1, 2, 3, 4};

    f2 a = v.xy;
    assert(equiv(a[0], 1.0f) && equiv(a[1], 2.0f));

    f2 b = v.zw;
    assert(equiv(b[0], 3.0f) && equiv(b[1], 4.0f));

    f2 c = v.wy;
    assert(equiv(c[0], 4.0f) && equiv(c[1], 2.0f));

    f2 d = v.xx;
    assert(equiv(d[0], 1.0f) && equiv(d[1], 1.0f));

    f2 e = v.ab;
    assert(equiv(e[0], 4.0f) && equiv(e[1], 3.0f));
}

static void test_swizzle_read_4() {
    f4 v = {1, 2, 3, 4};

    f4 r1 = v.wzyx;
    assert(equiv(r1[0], 4.0f) && equiv(r1[1], 3.0f) && equiv(r1[2], 2.0f) && equiv(r1[3], 1.0f));

    f4 r2 = v.xxxx;
    assert(equiv(r2[0], 1.0f) && equiv(r2[1], 1.0f) && equiv(r2[2], 1.0f) && equiv(r2[3], 1.0f));

    f4 r3 = v.argb;
    assert(equiv(r3[0], 4.0f) && equiv(r3[1], 1.0f) && equiv(r3[2], 2.0f) && equiv(r3[3], 3.0f));
}

static void test_swizzle_write_2() {
    f4 v = {1, 2, 3, 4};
    v.xy = f2{50, 60};
    assert(equiv(v[0], 50.0f) && equiv(v[1], 60.0f) && equiv(v[2], 3.0f) && equiv(v[3], 4.0f));

    v.zw = {70, 80};
    assert(equiv(v[2], 70.0f) && equiv(v[3], 80.0f));

    v.xy = 99.0f;
    assert(equiv(v[0], 99.0f) && equiv(v[1], 99.0f) && equiv(v[2], 70.0f) && equiv(v[3], 80.0f));
}

static void test_swizzle_write_4() {
    f4 v = 0.0f;
    v.wzyx = f4{1, 2, 3, 4};
    assert(equiv(v[0], 4.0f) && equiv(v[1], 3.0f) && equiv(v[2], 2.0f) && equiv(v[3], 1.0f));

    v.rgba = 5.0f;
    assert(equiv(v[0], 5.0f) && equiv(v[1], 5.0f) && equiv(v[2], 5.0f) && equiv(v[3], 5.0f));
}

static void test_swizzle_in_expression() {
    f4 v = {1, 2, 3, 4};

    f2 a = v.xy + v.zw;
    assert(equiv(a[0], 4.0f) && equiv(a[1], 6.0f));

    f4 b = v.wzyx * 2.0f;
    assert(equiv(b[0], 8.0f) && equiv(b[1], 6.0f) && equiv(b[2], 4.0f) && equiv(b[3], 2.0f));

    f4 c = v.wzyx + f4{10, 10, 10, 10};
    assert(equiv(c[0], 14.0f) && equiv(c[1], 13.0f) && equiv(c[2], 12.0f) && equiv(c[3], 11.0f));

    f4 d = f4{100, 100, 100, 100} - v.xyzw;
    assert(equiv(d[0], 99.0f) && equiv(d[3], 96.0f));

    f4 w = {0, 0, 0, 0};
    w.xy = v.zw;
    assert(equiv(w[0], 3.0f) && equiv(w[1], 4.0f) && equiv(w[2], 0.0f) && equiv(w[3], 0.0f));
}

static void test_swizzle_ints() {
    i4 v = {1, 2, 3, 4};
    i4 r = v.wzyx;
    assert(r[0] == 4 && r[3] == 1);

    v.xy = {-1, -2};
    assert(v[0] == -1 && v[1] == -2 && v[2] == 3 && v[3] == 4);
}

int main() {
    test_named_read();
    test_named_write();
    test_swizzle_read_2();
    test_swizzle_read_4();
    test_swizzle_write_2();
    test_swizzle_write_4();
    test_swizzle_in_expression();
    test_swizzle_ints();
    return 0;
}
