#include "../cve.h"
#include "test_util.h"

using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

namespace splat {
    constexpr f4 a = 2.5f;
    static_assert(equiv(a[0], 2.5f) && equiv(a[1], 2.5f)
               && equiv(a[2], 2.5f) && equiv(a[3], 2.5f));

    constexpr i4 b = -7;
    static_assert(b[0] == -7 && b[1] == -7 && b[2] == -7 && b[3] == -7);
}

namespace brace_init {
    constexpr f4 a = {1.0f, 2.0f, 3.0f, 4.0f};
    static_assert(equiv(a[0], 1.0f) && equiv(a[1], 2.0f)
               && equiv(a[2], 3.0f) && equiv(a[3], 4.0f));

    constexpr i4 b = {10, 20, 30, 40};
    static_assert(b[0] == 10 && b[3] == 40);
}

namespace vec_vec {
    constexpr f4 a = {1, 2, 3, 4};
    constexpr f4 b = {10, 20, 30, 40};

    constexpr f4 c = a + b;
    static_assert(equiv(c[0], 11.0f) && equiv(c[1], 22.0f)
               && equiv(c[2], 33.0f) && equiv(c[3], 44.0f));

    constexpr f4 d = b - a;
    static_assert(equiv(d[0], 9.0f) && equiv(d[3], 36.0f));

    constexpr f4 e = a * b;
    static_assert(equiv(e[0], 10.0f) && equiv(e[3], 160.0f));

    constexpr f4 f = b / a;
    static_assert(equiv(f[0], 10.0f) && equiv(f[1], 10.0f)
               && equiv(f[2], 10.0f) && equiv(f[3], 10.0f));
}

namespace vec_scalar {
    constexpr f4 a = {1, 2, 3, 4};

    constexpr f4 b = a + 1.0f;
    static_assert(equiv(b[0], 2.0f) && equiv(b[3], 5.0f));

    constexpr f4 c = 10.0f - a;
    static_assert(equiv(c[0], 9.0f) && equiv(c[3], 6.0f));

    constexpr f4 d = a * 2.0f;
    static_assert(equiv(d[0], 2.0f) && equiv(d[3], 8.0f));

    constexpr f4 e = a / 2.0f;
    static_assert(equiv(e[0], 0.5f) && equiv(e[3], 2.0f));
}

constexpr f4 compound_test() {
    f4 a = {1, 2, 3, 4};
    a += f4{10, 10, 10, 10};
    a *= 2.0f;
    a -= 2.0f;
    a /= 2.0f;
    return a;
}
static_assert(equiv(compound_test()[0], 10.0f) && equiv(compound_test()[3], 13.0f));

namespace unary {
    constexpr f4 a = {1, -2, 3, -4};
    constexpr f4 b = -a;
    static_assert(equiv(b[0], -1.0f) && equiv(b[1], 2.0f)
               && equiv(b[2], -3.0f) && equiv(b[3], 4.0f));
    constexpr f4 c = +a;
    static_assert(equiv(c[0], 1.0f) && equiv(c[1], -2.0f));
}

namespace mixed_expression {
    constexpr f4 a = {1, 2, 3, 4};
    constexpr f4 b = 2.0f;
    constexpr f4 c = (a + b) * 3.0f - a;
    static_assert(equiv(c[0], static_cast<float>((1+2)*3 - 1)));
    static_assert(equiv(c[1], static_cast<float>((2+2)*3 - 2)));
    static_assert(equiv(c[2], static_cast<float>((3+2)*3 - 3)));
    static_assert(equiv(c[3], static_cast<float>((4+2)*3 - 4)));
}

int main() { return 0; }
