#include "main.h"
#include "defines.h"
#include "compressor.h"
#include <math.h>

// https://github.com/YetAnotherElectronicsChannel/STM32_DSP_COMPRESSOR/

#define COMPRESSOR_ATTACK_MS   30
#define COMPRESSOR_RELEASE_MS  20
#define COMPRESSOR_HOLD_MS     10
#define COMPRESSOR_THRESHOLD   5000
#define COMPRESSOR_RATIO       3


void compressorInit(compressor_t *compressor)
{
  compressor->state = C_IDLE;
  compressor->attack = COMPRESSOR_ATTACK_MS * SAMPLING_FREQUENCY / 1000.0f;
  compressor->release = COMPRESSOR_RELEASE_MS * SAMPLING_FREQUENCY / 1000.0f;
  compressor->hold = COMPRESSOR_HOLD_MS * SAMPLING_FREQUENCY / 1000.0f;

  compressor->threshold = COMPRESSOR_THRESHOLD;
  compressor->gain_reduce = 1.0f / COMPRESSOR_RATIO;

  compressor->gain_step_attack = (1.0f - compressor->gain_reduce) / compressor->attack;
  compressor->gain_step_release = (1.0f - compressor->gain_reduce) / compressor->release;

  //initial gain = 1.0 -> no compression
  compressor->gain = 1.0f;
  compressor->flag = 0;
}


float compressorApply(compressor_t *compressor, float input)
{
  if (fabs(input) > compressor->threshold) {
    if (compressor->gain >=  compressor->gain_reduce) {
      if (compressor->state == C_IDLE) {
        compressor->state = C_ATTACK;
        compressor->timeout = compressor->attack;
      } else {
        if (compressor->state == C_RELEASE) {
          compressor->state = C_ATTACK;
          compressor->timeout = compressor->attack;
        }
      }
    }
    if (compressor->state == C_GAIN_REDUCE) {
      compressor->timeout = compressor->hold;
    }
  }

  if ((fabs(input) < compressor->threshold) && (compressor->gain <= 1.0f)) {
    if ((compressor->timeout == 0) && (compressor->state == C_GAIN_REDUCE)) {
      compressor->state = C_RELEASE;
      compressor->timeout = compressor->release;
    }
  }

  switch (compressor->state) {
    case C_ATTACK:
      if ((compressor->timeout > 0) && (compressor->gain > compressor->gain_reduce)) {
        compressor->gain -= compressor->gain_step_attack;
        compressor->timeout--;
      } else {
        compressor->state = C_GAIN_REDUCE;
        compressor->timeout = compressor->hold;
      }
      break;
    case C_GAIN_REDUCE:
      if (compressor->timeout > 0) {
        compressor->timeout--;
      } else {
        compressor->state = C_RELEASE;
        compressor->timeout = compressor->release;
      }
      break;

    case C_RELEASE:
      if (compressor->timeout>0 && compressor->gain<1.0f) {
        compressor->timeout--;
        compressor->gain += compressor->gain_step_release;
      } else {
        compressor->state=C_IDLE;
      }
      break;

    case C_IDLE:
      if (compressor->gain < 1.0f) {
        compressor->gain = 1.0f;
      }
      break;

    default:
      break;

  }
  if (compressor->state != C_IDLE) {
    compressor->flag = 1;
  }

  return input * compressor->gain;
}

