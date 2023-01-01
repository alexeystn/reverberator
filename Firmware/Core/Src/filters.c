#include "main.h"
#include "defines.h"
#include "filters.h"
#include <stdlib.h>
#include <string.h>


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

