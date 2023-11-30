#ifndef BIQUAD_H
#define BIQUAD_H

#include <stdint.h>

typedef struct biquadFilter_s {
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
} biquadFilter_t;

typedef enum {
    FILTER_LPF,
    FILTER_HPF,
    FILTER_NOTCH,
    FILTER_BPF,
} biquadFilterType_e;


void biquadFilterInit(biquadFilter_t *filter, float filterFreq, biquadFilterType_e filterType);
void biquadFilterUpdate(biquadFilter_t *filter, float filterFreq, biquadFilterType_e filterType);

float biquadFilterApply(biquadFilter_t *filter, float input);

#endif
