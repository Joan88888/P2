#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float power = 1.0e-15f;

    for (int i = 0; i<N; i++) {
        power += pow(x[i],2);
    }

    return 10 * log10f(power / N);
}

float compute_am(const float *x, unsigned int N) {
    float am = 0.0f;

    for (int i = 0; i < N; i++) {
        am += fabs(x[i]);
    }

    return am / N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    float zcr = 0.0f;
    const float k = fm / (2 * (N - 1));

    for (int i = 1; i < N; i++) {
        if (x[i]*x[i-1] < 0) 
            zcr++;
    }

    return k * zcr;
}
