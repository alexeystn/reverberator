#include "main.h"
#include "defines.h"
#include "compressor.h"
#include <math.h>

// https://github.com/YetAnotherElectronicsChannel/STM32_DSP_COMPRESSOR/

#define COMPRESSOR_ATTACK_MS   30
#define COMPRESSOR_RELEASE_MS  20
#define COMPRESSOR_HOLD_MS     10
#define COMPRESSOR_THRESHOLD   3000
#define COMPRESSOR_RATIO       5


enum comp_state_t {
  C_IDLE,
  C_ATTACK,
  C_GAIN_REDUCE,
  C_RELEASE };

enum comp_state_t state = C_IDLE;


int32_t attack, release, hold, timeout;
float gain_reduce, gain_step_attack, gain_step_release, gain, threshold;


void Compressor_Init(void)
{
  attack = COMPRESSOR_ATTACK_MS * SAMPLING_FREQUENCY / 1000.0f;
  release = COMPRESSOR_RELEASE_MS * SAMPLING_FREQUENCY / 1000.0f;
  hold = COMPRESSOR_HOLD_MS * SAMPLING_FREQUENCY / 1000.0f;

  threshold = COMPRESSOR_THRESHOLD;
  gain_reduce = 1.0f / COMPRESSOR_RATIO;

  gain_step_attack = (1.0f - gain_reduce) / attack;
  gain_step_release = (1.0f - gain_reduce) / release;

  //initial gain = 1.0 -> no compression
  gain = 1.0f;
}


float Compressor_Do(float sample)
{
  if (fabs(sample) > threshold) {
    if (gain >=  gain_reduce) {
      if (state == C_IDLE) {
        state = C_ATTACK;
        timeout = attack;
      } else {
        if (state == C_RELEASE) {
          state = C_ATTACK;
          timeout = attack;
        }
      }
    }
    if (state == C_GAIN_REDUCE) {
      timeout = hold;
    }
  }

  if ((fabs(sample) < threshold) && (gain <= 1.0f)) {
    if ((timeout == 0) && (state == C_GAIN_REDUCE)) {
      state = C_RELEASE;
      timeout = release;
    }
  }

  switch (state) {
    case C_ATTACK:
      if ((timeout > 0) && (gain > gain_reduce)) {
        gain -= gain_step_attack;
        timeout--;
      } else {
        state = C_GAIN_REDUCE;
        timeout = hold;
      }
      break;
    case C_GAIN_REDUCE:
      if (timeout > 0) {
        timeout--;
      } else {
        state = C_RELEASE;
        timeout = release;
      }
      break;

    case C_RELEASE:
      if (timeout>0 && gain<1.0f) {
        timeout--;
        gain += gain_step_release;
      } else {
        state=C_IDLE;
      }
      break;

    case C_IDLE:
      if (gain < 1.0f) {
        gain = 1.0f;
      }
      break;

    default:
      break;

  }

  if (state == C_IDLE) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  }

  return sample * gain;
}
