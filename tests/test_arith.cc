#include "cve.h"

#include <cassert>

using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

static void test_splat() {
    f4 a = 2.5f;
    for (int i = 0; i < 4; ++i) assert(a[i] == 2.5f);

    i4 b = -7;
    for (int i = 0; i < 4; ++i) assert(b[i] == -7);
}

static void test_brace_init() {
    f4 a = {1.0f, 2.0f, 3.0f, 4.0f};
    assert(a[0] == 1.0f);
    assert(a[1] == 2.0f);
    assert(a[2] == 3.0f);
    assert(a[3] == 4.0f);

    i4 b = {10, 20, 30, 40};
    assert(b[0] == 10);
    assert(b[3] == 40);
}

static void test_vec_vec() {
    f4 a = {1, 2, 3, 4};
    f4 b = {10, 20, 30, 40};

    f4 c = a + b;
    assert(c[0] == 11 && c[1] == 22 && c[2] == 33 && c[3] == 44);

    f4 d = b - a;
    assert(d[0] == 9 && d[3] == 36);

    f4 e = a * b;
    assert(e[0] == 10 && e[3] == 160);

    f4 f = b / a;
    assert(f[0] == 10 && f[1] == 10 && f[2] == 10 && f[3] == 10);
}

static void test_vec_scalar() {
    f4 a = {1, 2, 3, 4};

    f4 b = a + 1.0f;
    assert(b[0] == 2 && b[3] == 5);

    f4 c = 10.0f - a;
    assert(c[0] == 9 && c[3] == 6);

    f4 d = a * 2.0f;
    assert(d[0] == 2 && d[3] == 8);

    f4 e = a / 2.0f;
    assert(e[0] == 0.5f && e[3] == 2.0f);
}

static void test_compound() {
    f4 a = {1, 2, 3, 4};
    a += f4{10, 10, 10, 10};
    assert(a[0] == 11 && a[3] == 14);
    a *= 2.0f;
    assert(a[0] == 22 && a[3] == 28);
    a -= 2.0f;
    assert(a[0] == 20 && a[3] == 26);
    a /= 2.0f;
    assert(a[0] == 10 && a[3] == 13);
}

static void test_unary() {
    f4 a = {1, -2, 3, -4};
    f4 b = -a;
    assert(b[0] == -1 && b[1] == 2 && b[2] == -3 && b[3] == 4);
    f4 c = +a;
    assert(c[0] == 1 && c[1] == -2);
}

static void test_mixed_expression() {
    f4 a = {1, 2, 3, 4};
    f4 b = 2.0f;
    f4 c = (a + b) * 3.0f - a;
    assert(c[0] == (1+2)*3 - 1);
    assert(c[1] == (2+2)*3 - 2);
    assert(c[2] == (3+2)*3 - 3);
    assert(c[3] == (4+2)*3 - 4);
}

int main() {
    test_splat();
    test_brace_init();
    test_vec_vec();
    test_vec_scalar();
    test_compound();
    test_unary();
    test_mixed_expression();
    return 0;
}
