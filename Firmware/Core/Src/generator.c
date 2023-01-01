#include "main.h"
#include "defines.h"
#include "math.h"

#define TICKS_IN_PERIOD  (SAMPLING_FREQUENCY / GENERATOR_FREQUENCY_HZ)

int16_t generator_output = 0;
uint32_t generator_counter = 0;


uint16_t Generator_GetValue(void)
{
  return generator_output;
}


void Generator_Step(void)
{
  float y = 0;
  y = -fabs(((float)generator_counter) / TICKS_IN_PERIOD - 0.5f) * 4.0f + 1.0f;
  generator_output = y * GENERATOR_AMPLITUDE;
  generator_counter++;
  if (generator_counter == TICKS_IN_PERIOD) {
    generator_counter = 0;
  }
}
