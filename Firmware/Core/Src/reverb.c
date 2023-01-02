#include "main.h"
#include "defines.h"
#include "reverb.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef enum {
  F_COMB,
  F_ALLPASS
} filter_type_t;


typedef struct {
  filter_type_t type;
  float *buffer;
  uint32_t length;
  uint32_t pointer;
  float gain;
} filter_t;

#define F_COMB_COUNT        4
#define F_ALLPASS_COUNT    3


void Filter_Init(filter_t *filter, filter_type_t type, float gain, float delay_ms);
float Filter_Process(filter_t *filter, float sample);

void Filter_Init(filter_t *filter, filter_type_t type, float gain, float delay_ms)
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


float Filter_Process(filter_t *filter, float sample)
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


filter_t Filters_Comb[F_COMB_COUNT];
filter_t Filters_AllPass[F_ALLPASS_COUNT];


void Reverb_Init(void)
{
  Filter_Init(&Filters_Comb[0], F_COMB, 0.805, 36.04);
  Filter_Init(&Filters_Comb[1], F_COMB, 0.827, 31.12);
  Filter_Init(&Filters_Comb[2], F_COMB, 0.783, 40.44);
  Filter_Init(&Filters_Comb[3], F_COMB, 0.764, 44.92);
  Filter_Init(&Filters_AllPass[0], F_ALLPASS, 0.7, 5.00);
  Filter_Init(&Filters_AllPass[1], F_ALLPASS, 0.7, 1.68);
  Filter_Init(&Filters_AllPass[2], F_ALLPASS, 0.7, 0.48);

#if 1  // extend reverb time
  uint8_t i;
  for (i = 0; i < F_COMB_COUNT; i++) {
    Filters_Comb[i].gain = sqrtf(Filters_Comb[i].gain);
  }
  for (i = 0; i < F_ALLPASS_COUNT; i++) {
    Filters_AllPass[i].gain = sqrtf(Filters_AllPass[i].gain);
  }
#endif
}


float Reverb_Do(float sample)
{
  float newSample = 0;
  uint8_t i;
  for (i = 0; i < F_COMB_COUNT; i++) {
    newSample += Filter_Process(&Filters_Comb[i], sample);
  }
  newSample /= 4.0f;

  for (i = 0; i < F_ALLPASS_COUNT; i++) {
    newSample = Filter_Process(&Filters_AllPass[i], newSample);
  }
  return newSample;
}
