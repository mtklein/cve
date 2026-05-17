#include "../cve.h"

using f4 = cve<float, 4>;

f4 fma(f4 a, f4 b, f4 c) {
    return a * b + c;
}
