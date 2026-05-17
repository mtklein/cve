#include "cve.h"

#include <cassert>

using f2 = cve<float, 2>;
using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

static void test_named_read() {
    f4 v = {10, 20, 30, 40};
    assert(v.x == 10 && v.y == 20 && v.z == 30 && v.w == 40);
    assert(v.r == 10 && v.g == 20 && v.b == 30 && v.a == 40);

    f2 p = {7, 8};
    assert(p.x == 7 && p.y == 8);
    assert(p.r == 7 && p.g == 8);
}

static void test_named_write() {
    f4 v = 0.0f;
    v.x = 1; v.y = 2; v.z = 3; v.w = 4;
    assert(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);

    v.r = 9;
    assert(v[0] == 9);
}

static void test_swizzle_read_2() {
    f4 v = {1, 2, 3, 4};

    f2 a = v.xy;
    assert(a[0] == 1 && a[1] == 2);

    f2 b = v.zw;
    assert(b[0] == 3 && b[1] == 4);

    f2 c = v.wy;
    assert(c[0] == 4 && c[1] == 2);

    f2 d = v.xx;
    assert(d[0] == 1 && d[1] == 1);

    f2 e = v.ab;
    assert(e[0] == 4 && e[1] == 3);
}

static void test_swizzle_read_4() {
    f4 v = {1, 2, 3, 4};

    f4 r1 = v.wzyx;
    assert(r1[0] == 4 && r1[1] == 3 && r1[2] == 2 && r1[3] == 1);

    f4 r2 = v.xxxx;
    assert(r2[0] == 1 && r2[1] == 1 && r2[2] == 1 && r2[3] == 1);

    f4 r3 = v.argb;
    assert(r3[0] == 4 && r3[1] == 1 && r3[2] == 2 && r3[3] == 3);
}

static void test_swizzle_write_2() {
    f4 v = {1, 2, 3, 4};
    v.xy = f2{50, 60};
    assert(v[0] == 50 && v[1] == 60 && v[2] == 3 && v[3] == 4);

    v.zw = {70, 80};
    assert(v[2] == 70 && v[3] == 80);

    v.xy = 99.0f;
    assert(v[0] == 99 && v[1] == 99 && v[2] == 70 && v[3] == 80);
}

static void test_swizzle_write_4() {
    f4 v = 0.0f;
    v.wzyx = f4{1, 2, 3, 4};
    assert(v[0] == 4 && v[1] == 3 && v[2] == 2 && v[3] == 1);

    v.rgba = 5.0f;
    assert(v[0] == 5 && v[1] == 5 && v[2] == 5 && v[3] == 5);
}

static void test_swizzle_in_expression() {
    f4 v = {1, 2, 3, 4};

    f2 a = v.xy + v.zw;
    assert(a[0] == 4 && a[1] == 6);

    f4 b = v.wzyx * 2.0f;
    assert(b[0] == 8 && b[1] == 6 && b[2] == 4 && b[3] == 2);

    f4 c = v.wzyx + f4{10, 10, 10, 10};
    assert(c[0] == 14 && c[1] == 13 && c[2] == 12 && c[3] == 11);

    f4 d = f4{100, 100, 100, 100} - v.xyzw;
    assert(d[0] == 99 && d[3] == 96);

    f4 w = {0, 0, 0, 0};
    w.xy = v.zw;
    assert(w[0] == 3 && w[1] == 4 && w[2] == 0 && w[3] == 0);
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
