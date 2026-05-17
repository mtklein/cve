#include "../cve.h"

using f8 = cve<float, 8>;
using i8 = cve<int32_t, 8>;

f8 min(f8 a, f8 b)          { return cve_min(a, b); }
f8 max(f8 a, f8 b)          { return cve_max(a, b); }
f8 abs(f8 a)                { return cve_abs(a); }
f8 sqrt(f8 a)               { return cve_sqrt(a); }
f8 floor(f8 a)              { return cve_floor(a); }
f8 ceil(f8 a)               { return cve_ceil(a); }
f8 round(f8 a)              { return cve_round(a); }
f8 fma(f8 a, f8 b, f8 c)    { return cve_fma(a, b, c); }
f8 reverse(f8 a)            { return cve_shuffle<7,6,5,4,3,2,1,0>(a); }
i8 to_i32(f8 a)             { return cve_convert<int32_t>(a); }
