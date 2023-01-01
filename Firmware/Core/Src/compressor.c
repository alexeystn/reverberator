#include "main.h"
#include "defines.h"
#include "compressor.h"
#include "math.h"

// https://github.com/YetAnotherElectronicsChannel/STM32_DSP_COMPRESSOR/


// 0 = no gain reduction
// 1 = attack
// 2 = gain reduction
// 3 = release
enum CompStates {S_NoOperation, S_Attack, S_GainReduction, S_Release };
enum CompStates State = S_NoOperation;


int32_t Attack, Release, Hold, TimeOut;
float gainreduce, gain_step_attack, gain_step_release, gain, threshold;


void Compressor_Init(void)
{
  // 1 sample = 1/96kHz = ~10us
  //Attack -> 30 ms -> 3000
  //Release -> 20 ms -> 2000
  //Hold -> 10ms -> 1000
  Attack = 30 * SAMPLING_FREQUENCY / 1000.0f;
  Release = 20 * SAMPLING_FREQUENCY / 1000.0f;
  Hold = 10 * SAMPLING_FREQUENCY / 1000.0f;

  //threshold -20dB below limit -> 0.1 * 2^31
  threshold = 3000.0f; ////0.1f * 2147483648;

  //compression ratio: 6:1 -> -6dB = 0.5
  gainreduce = 0.5f;

  gain_step_attack = (1.0f - gainreduce) / Attack;
  gain_step_release = (1.0f - gainreduce) / Release;

  //initial gain = 1.0 -> no compression
  gain = 1.0f;
}


float Compressor_Do(float inSample)
{
  float inSampleF = (float)inSample;

  if (fabs(inSampleF) > threshold) {
    if (gain >=  gainreduce) {
      if (State==S_NoOperation) {
        State=S_Attack;
        TimeOut = Attack;
      }
      else if (State==S_Release) {
        State=S_Attack;
        TimeOut = Attack;
      }
    }
    if (State==S_GainReduction) TimeOut = Hold;

  }

  if (fabs(inSampleF) < threshold && gain <= 1.0f) {
    if (TimeOut==0 && State==S_GainReduction) {
      State=S_Release;
      TimeOut = Release;
    }
  }
  switch (State) {

    case S_Attack:
      if (TimeOut>0 && gain > gainreduce) {
        gain -= gain_step_attack;
        TimeOut--;
      }
      else {
        State=S_GainReduction;
        TimeOut = Hold;
      }
      break;


    case S_GainReduction:
      if (TimeOut>0) TimeOut--;
      else {
        State=S_Release;
        TimeOut = Release;
      }
      break;


    case S_Release:
      if (TimeOut>0 && gain<1.0f) {
        TimeOut--;
        gain += gain_step_release;
      }
      else {
        State=S_NoOperation;
      }
      break;

    case S_NoOperation:
      if (gain < 1.0f) gain = 1.0F;
      break;

    default:

      break;

  }

  if (State == S_NoOperation) {
    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  } else {
    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  }

  float outSampleF = inSample*gain;

  return (int) outSampleF;
}
