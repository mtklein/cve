#include "../cve.h"
#include "test_util.h"

#include <cassert>
#include <cstdint>

using f4 = cve<float, 4>;
using i4 = cve<int32_t, 4>;

static void test_min_max() {
    f4 a = {1.0f, 5.0f, -2.0f, 4.0f};
    f4 b = {3.0f, 2.0f,  0.0f, 4.0f};
    f4 mn = cve_min(a, b);
    f4 mx = cve_max(a, b);
    assert(equiv(mn[0], 1.0f) && equiv(mn[1], 2.0f) && equiv(mn[2], -2.0f) && equiv(mn[3], 4.0f));
    assert(equiv(mx[0], 3.0f) && equiv(mx[1], 5.0f) && equiv(mx[2],  0.0f) && equiv(mx[3], 4.0f));

    i4 ai = {1, 5, -2, 4};
    i4 bi = {3, 2,  0, 4};
    i4 mni = cve_min(ai, bi);
    i4 mxi = cve_max(ai, bi);
    assert(mni[0] == 1 && mni[1] == 2 && mni[2] == -2 && mni[3] == 4);
    assert(mxi[0] == 3 && mxi[1] == 5 && mxi[2] ==  0 && mxi[3] == 4);
}

static void test_abs() {
    f4 a = {-1.5f, 2.5f, -0.0f, 4.0f};
    f4 r = cve_abs(a);
    assert(equiv(r[0], 1.5f) && equiv(r[1], 2.5f) && equiv(r[3], 4.0f));

    i4 ai = {-7, 3, -100, 0};
    i4 ri = cve_abs(ai);
    assert(ri[0] == 7 && ri[1] == 3 && ri[2] == 100 && ri[3] == 0);
}

static void test_sqrt() {
    f4 a = {4.0f, 9.0f, 16.0f, 25.0f};
    f4 r = cve_sqrt(a);
    assert(equiv(r[0], 2.0f) && equiv(r[1], 3.0f) && equiv(r[2], 4.0f) && equiv(r[3], 5.0f));
}

static void test_floor_ceil_round() {
    f4 a = {1.5f, 2.7f, -3.2f, 4.0f};
    f4 fl = cve_floor(a);
    f4 ce = cve_ceil(a);
    f4 rn = cve_round(a);
    assert(equiv(fl[0], 1.0f) && equiv(fl[1], 2.0f) && equiv(fl[2], -4.0f) && equiv(fl[3], 4.0f));
    assert(equiv(ce[0], 2.0f) && equiv(ce[1], 3.0f) && equiv(ce[2], -3.0f) && equiv(ce[3], 4.0f));
    assert(equiv(rn[0], 2.0f) && equiv(rn[1], 3.0f) && equiv(rn[2], -3.0f) && equiv(rn[3], 4.0f));
}

static void test_fma() {
    f4 a = {2, 3, 4, 5};
    f4 b = {10, 10, 10, 10};
    f4 c = {1, 1, 1, 1};
    f4 r = cve_fma(a, b, c);  // a*b + c = {21, 31, 41, 51}
    assert(equiv(r[0], 21.0f) && equiv(r[1], 31.0f) && equiv(r[2], 41.0f) && equiv(r[3], 51.0f));
}

int main() {
    test_min_max();
    test_abs();
    test_sqrt();
    test_floor_ceil_round();
    test_fma();
    return 0;
}
