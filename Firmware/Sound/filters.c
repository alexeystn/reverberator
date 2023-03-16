#include "main.h"
#include "defines.h"
#include "filters.h"
#include "biquad.h"
#include <math.h>

#define FILTER_FREQUENCY_HPF    400
#define FILTER_FREQUENCY_LPF    3000
#define FILTER_FREQUENCY_NOTCH  50

biquadFilter_t filterHigh, filterLow;

void Filters_Init(void)
{
  biquadFilterInit(&filterHigh,
                   FILTER_FREQUENCY_HPF,
                   1e6 / SAMPLING_FREQUENCY,
                   1.0f / sqrtf(2.0f),
                   FILTER_HPF);

  biquadFilterInit(&filterLow,
                   FILTER_FREQUENCY_LPF,
                   1e6 / SAMPLING_FREQUENCY,
                   1.0f / sqrtf(2.0f),
                   FILTER_LPF);
}


float Filters_Do(float sample)
{
  sample = biquadFilterApply(&filterHigh, sample);
  sample = biquadFilterApply(&filterLow, sample);
  return sample;
}
