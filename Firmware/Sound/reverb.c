#include "main.h"
#include "defines.h"
#include "reverb.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


//static void Filter_Init(filter_t *filter, filter_type_t type, float gain, float delay_ms);
//static float Filter_Process(filter_t *filter, float sample);


static void Filter_Init(filter_t *filter, filter_type_t type, float gain, float delay_ms)
{
  filter->type = type;
  filter->pointer = 0;
  filter->gain = gain;
  filter->length = (int)(SAMPLING_FREQUENCY * delay_ms / 1000.0f);
  filter->buffer = (float *)malloc(  sizeof(float) * filter->length);
  if (filter->buffer == NULL) {
    while (1) {
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      HAL_Delay(100);
    }
  } else {
    memset(filter->buffer, 0, sizeof(float) * filter->length);
  }
}


static float Filter_Process(filter_t *filter, float sample)
{
  float new;
  float readback = filter->buffer[filter->pointer];
  if (filter->type == F_COMB) {
    new = readback * filter->gain + sample;
  } else {
    readback += (-filter->gain) * sample;
    new = readback*filter->gain + sample;
  }
  filter->buffer[filter->pointer] = new;
  filter->pointer++;
  if (filter->pointer == filter->length) {
    filter->pointer = 0;
  }
  return readback;
}


void reverbInit(reverb_t *reverb)
{
  Filter_Init(&reverb->filtersComb[0], F_COMB, 0.805, 36.04);
  Filter_Init(&reverb->filtersComb[1], F_COMB, 0.827, 31.12);
  Filter_Init(&reverb->filtersComb[2], F_COMB, 0.783, 40.44);
  Filter_Init(&reverb->filtersComb[3], F_COMB, 0.764, 44.92);
  Filter_Init(&reverb->filtersAllPass[0], F_ALLPASS, 0.7, 5.00);
  Filter_Init(&reverb->filtersAllPass[1], F_ALLPASS, 0.7, 1.68);
  Filter_Init(&reverb->filtersAllPass[2], F_ALLPASS, 0.7, 0.48);

#if 1  // extend reverb time
  uint8_t i;
  for (i = 0; i < F_COMB_COUNT; i++) {
    reverb->filtersComb[i].gain = sqrtf(reverb->filtersComb[i].gain);
  }
  for (i = 0; i < F_ALLPASS_COUNT; i++) {
    reverb->filtersAllPass[i].gain = sqrtf(reverb->filtersAllPass[i].gain);
  }
#endif
}


float reverbApply(reverb_t *reverb, float sample)
{
  float newSample = 0;
  uint8_t i;
  for (i = 0; i < F_COMB_COUNT; i++) {
    newSample += Filter_Process(&reverb->filtersComb[i], sample);
  }
  newSample /= 4.0f;

  for (i = 0; i < F_ALLPASS_COUNT; i++) {
    newSample = Filter_Process(&reverb->filtersAllPass[i], newSample);
  }
  return newSample;
}
