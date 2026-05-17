#include "cve.h"
#include "test_util.h"

#include <cassert>

using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

static void test_splat() {
    f4 a = 2.5f;
    for (std::size_t i = 0; i < 4; ++i) assert(equiv(a[i], 2.5f));

    i4 b = -7;
    for (std::size_t i = 0; i < 4; ++i) assert(b[i] == -7);
}

static void test_brace_init() {
    f4 a = {1.0f, 2.0f, 3.0f, 4.0f};
    assert(equiv(a[0], 1.0f));
    assert(equiv(a[1], 2.0f));
    assert(equiv(a[2], 3.0f));
    assert(equiv(a[3], 4.0f));

    i4 b = {10, 20, 30, 40};
    assert(b[0] == 10);
    assert(b[3] == 40);
}

static void test_vec_vec() {
    f4 a = {1, 2, 3, 4};
    f4 b = {10, 20, 30, 40};

    f4 c = a + b;
    assert(equiv(c[0], 11.0f) && equiv(c[1], 22.0f) && equiv(c[2], 33.0f) && equiv(c[3], 44.0f));

    f4 d = b - a;
    assert(equiv(d[0], 9.0f) && equiv(d[3], 36.0f));

    f4 e = a * b;
    assert(equiv(e[0], 10.0f) && equiv(e[3], 160.0f));

    f4 f = b / a;
    assert(equiv(f[0], 10.0f) && equiv(f[1], 10.0f) && equiv(f[2], 10.0f) && equiv(f[3], 10.0f));
}

static void test_vec_scalar() {
    f4 a = {1, 2, 3, 4};

    f4 b = a + 1.0f;
    assert(equiv(b[0], 2.0f) && equiv(b[3], 5.0f));

    f4 c = 10.0f - a;
    assert(equiv(c[0], 9.0f) && equiv(c[3], 6.0f));

    f4 d = a * 2.0f;
    assert(equiv(d[0], 2.0f) && equiv(d[3], 8.0f));

    f4 e = a / 2.0f;
    assert(equiv(e[0], 0.5f) && equiv(e[3], 2.0f));
}

static void test_compound() {
    f4 a = {1, 2, 3, 4};
    a += f4{10, 10, 10, 10};
    assert(equiv(a[0], 11.0f) && equiv(a[3], 14.0f));
    a *= 2.0f;
    assert(equiv(a[0], 22.0f) && equiv(a[3], 28.0f));
    a -= 2.0f;
    assert(equiv(a[0], 20.0f) && equiv(a[3], 26.0f));
    a /= 2.0f;
    assert(equiv(a[0], 10.0f) && equiv(a[3], 13.0f));
}

static void test_unary() {
    f4 a = {1, -2, 3, -4};
    f4 b = -a;
    assert(equiv(b[0], -1.0f) && equiv(b[1], 2.0f) && equiv(b[2], -3.0f) && equiv(b[3], 4.0f));
    f4 c = +a;
    assert(equiv(c[0], 1.0f) && equiv(c[1], -2.0f));
}

static void test_mixed_expression() {
    f4 a = {1, 2, 3, 4};
    f4 b = 2.0f;
    f4 c = (a + b) * 3.0f - a;
    assert(equiv(c[0], static_cast<float>((1+2)*3 - 1)));
    assert(equiv(c[1], static_cast<float>((2+2)*3 - 2)));
    assert(equiv(c[2], static_cast<float>((3+2)*3 - 3)));
    assert(equiv(c[3], static_cast<float>((4+2)*3 - 4)));
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
