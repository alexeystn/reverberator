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


float compressorApply(compressor_t *c, float input)
{
  if (fabs(input) > c->threshold) {
    if (c->gain >=  c->gain_reduce) {
      if (c->state == C_IDLE) {
        c->state = C_ATTACK;
        c->timeout = c->attack;
      } else {
        if (c->state == C_RELEASE) {
          c->state = C_ATTACK;
          c->timeout = c->attack;
        }
      }
    }
    if (c->state == C_GAIN_REDUCE) {
      c->timeout = c->hold;
    }
  }

  if ((fabs(input) < c->threshold) && (c->gain <= 1.0f)) {
    if ((c->timeout == 0) && (c->state == C_GAIN_REDUCE)) {
      c->state = C_RELEASE;
      c->timeout = c->release;
    }
  }

  switch (c->state) {
    case C_ATTACK:
      if ((c->timeout > 0) && (c->gain > c->gain_reduce)) {
        c->gain -= c->gain_step_attack;
        c->timeout--;
      } else {
        c->state = C_GAIN_REDUCE;
        c->timeout = c->hold;
      }
      break;
    case C_GAIN_REDUCE:
      if (c->timeout > 0) {
        c->timeout--;
      } else {
        c->state = C_RELEASE;
        c->timeout = c->release;
      }
      break;

    case C_RELEASE:
      if ((c->timeout > 0) && (c->gain < 1.0f)) {
        c->timeout--;
        c->gain += c->gain_step_release;
      } else {
        c->state=C_IDLE;
      }
      break;

    case C_IDLE:
      if (c->gain < 1.0f) {
        c->gain = 1.0f;
      }
      break;

    default:
      break;

  }
  if (c->state != C_IDLE) {
    c->flag = 1;
  }

  return input * c->gain;
}

