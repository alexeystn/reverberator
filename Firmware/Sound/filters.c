#include "main.h"
#include "defines.h"
#include "filters.h"
#include "filter.h"

#define FILTER_FREQUENCY_LPF    3000
#define FILTER_FREQUENCY_NOTCH  50

biquadFilter_t filterLPF;
biquadFilter_t filterNotch;


void Filters_Init(void)
{
  biquadFilterInitLPF(&filterLPF, FILTER_FREQUENCY_LPF, 1e6 / SAMPLING_FREQUENCY);
  biquadFilterInit(&filterNotch, FILTER_FREQUENCY_NOTCH, 1e6 / SAMPLING_FREQUENCY, 2.0f, FILTER_NOTCH, 1.0f);
}


float Filters_Do(float sample)
{
  sample = biquadFilterApply(&filterLPF, sample);
  sample = biquadFilterApply(&filterNotch, sample);
  return sample;
}
