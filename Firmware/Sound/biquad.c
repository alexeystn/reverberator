#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "defines.h"
#include "biquad.h"

#define BIQUAD_Q 1.0f / sqrtf(2.0f)     /* quality factor - 2nd order butterworth*/

#define M_PIf  3.1415926f

#define sin_approx  sin
#define cos_approx  cos


void biquadFilterInit(biquadFilter_t *filter, float filterFreq, biquadFilterType_e filterType)
{
    const uint32_t refreshRate = 1e6 / SAMPLING_FREQUENCY;
    const float omega = 2.0f * M_PIf * filterFreq * refreshRate * 0.000001f;
    const float sn = sin_approx(omega);
    const float cs = cos_approx(omega);
    const float Q = 1.0f / sqrtf(2.0f);
    const float alpha = sn / (2.0f * Q);

    switch (filterType) {
    case FILTER_LPF:
        // http://www.ti.com/lit/an/slaa447/slaa447.pdf
        filter->b1 = 1 - cs;
        filter->b0 = filter->b1 * 0.5f;
        filter->b2 = filter->b0;
        filter->a1 = -2 * cs;
        filter->a2 = 1 - alpha;
        break;
    case FILTER_HPF:
        filter->b1 = -(1 + cs);
        filter->b0 = filter->b1 * (-0.5f);
        filter->b2 = filter->b0;
        filter->a1 = -2 * cs;
        filter->a2 = 1 - alpha;
        break;
    case FILTER_NOTCH:
        filter->b0 = 1;
        filter->b1 = -2 * cs;
        filter->b2 = 1;
        filter->a1 = filter->b1;
        filter->a2 = 1 - alpha;
        break;
    case FILTER_BPF:
        filter->b0 = alpha;
        filter->b1 = 0;
        filter->b2 = -alpha;
        filter->a1 = -2 * cs;
        filter->a2 = 1 - alpha;
        break;
    }

    const float a0 = 1 + alpha;

    filter->b0 /= a0;
    filter->b1 /= a0;
    filter->b2 /= a0;
    filter->a1 /= a0;
    filter->a2 /= a0;

    filter->x1 = filter->x2 = 0;
    filter->y1 = filter->y2 = 0;
}


float biquadFilterApply(biquadFilter_t *filter, float input)
{
    const float result = filter->b0 * input + filter->x1;

    filter->x1 = filter->b1 * input - filter->a1 * result + filter->x2;
    filter->x2 = filter->b2 * input - filter->a2 * result;

    return result;
}
