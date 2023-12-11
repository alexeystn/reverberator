#ifdef USE_HAL_DRIVER
#include "main.h"
#endif
#include "defines.h"
#include "compressor.h"
#include <math.h>

#define COMPRESSOR_ATTACK_MS   30
#define COMPRESSOR_RELEASE_MS  20
#define COMPRESSOR_HOLD_MS     10


void compressorInit(compressor_t *c, float threshold_dB, float ratio)
{
  c->state = 0;
  c->attack_smps = COMPRESSOR_ATTACK_MS * SAMPLING_FREQUENCY / 1000.0f;
  c->release_smps = COMPRESSOR_RELEASE_MS * SAMPLING_FREQUENCY / 1000.0f;
  c->hold_smps = COMPRESSOR_HOLD_MS * SAMPLING_FREQUENCY / 1000.0f;
  c->gain_step_attack = (1.0f - 1.0f / ratio) / c->attack_smps;
  c->gain_step_release = (1.0f - 1.0f / ratio) / c->release_smps;
  c->gain_current = 1.0f; //initial - no compression
  c->envelope = c->threshold;
  c->state = 0;
  compressorUpdate(c, threshold_dB, ratio);
}

void compressorUpdate(compressor_t *c, float threshold_dB, float ratio)
{
  c->ratio = ratio;
  c->threshold = (OVERLOAD_LIMIT / 2) * threshold_dB;
  //float maxCompressorOutput;
  //maxCompressorOutput = c->threshold + (OVERLOAD_LIMIT - c->threshold) / ratio;
  //c->restore_gain = OVERLOAD_LIMIT / maxCompressorOutput;
}


float compressorApply(compressor_t *c, float input)
{
  float input_abs = fabs(input);
  uint8_t need_to_recalc = 0;
  if (input_abs > c->threshold) {
    c->state = 1;
    c->timeout = c->hold_smps;
    if (input_abs >= c->envelope) {
      c->envelope = input_abs;
      need_to_recalc = 1;
    }
  } else {
    if (c->timeout) {
      c->timeout--;
      if (c->timeout == 0) {
        c->envelope = c->threshold;
        c->state = 0;
        need_to_recalc = 0;
      }
    }
  }

  float target_gain = 1;
  if (c->state) {
    float desired_level = c->threshold + (c->envelope - c->threshold) / c->ratio;
    target_gain = desired_level / c->envelope;
  }

  if (need_to_recalc) {
    c->gain_step_attack = (1.0f - target_gain) / c->attack_smps;
    c->gain_step_release = (1.0f - target_gain) / c->release_smps;
    need_to_recalc = 0;
  }

  if (c->state) {
    if (c->gain_current > target_gain) {
      c->gain_current -= c->gain_step_attack;
    } else {
      c->gain_current = target_gain;
    }
  } else {
    if (c->gain_current < 1) {
      c->gain_current += c->gain_step_release;
    } else {
      c->gain_current = 1;
    }
  }

  //return (input * c->gain_current * c->restore_gain);
  return (input * c->gain_current);
}


uint8_t limiterApply(float *sample)
{
  uint8_t ret = 0;
  if (*sample >= OVERLOAD_LIMIT) {
    *sample = OVERLOAD_LIMIT - 1;
  }
  if (*sample <= -OVERLOAD_LIMIT) {
    *sample = -OVERLOAD_LIMIT + 1;
  }
  return ret;
}
