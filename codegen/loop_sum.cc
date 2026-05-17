#include "../cve.h"

using f4 = cve<float, 4>;

float sum(const f4* p, int n) {
    f4 acc = 0.0f;
    for (int i = 0; i < n; ++i) {
        acc += p[i];
    }
    return acc[0] + acc[1] + acc[2] + acc[3];
}
