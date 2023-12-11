#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <stdint.h>

enum comp_state_t {
  C_IDLE,
  C_ATTACK,
  C_GAIN_REDUCE,
  C_RELEASE
};


typedef struct compressor_s {
  int32_t attack_smps;
  int32_t release_smps;
  int32_t hold_smps;
  int32_t timeout;
  float gain_step_attack;
  float gain_step_release;
  float gain_current;
  float envelope;
  float ratio;
  float threshold;
  float restore_gain;
  uint8_t state;
} compressor_t;


void compressorInit(compressor_t *compressor, float threshold_dB, float ratio);
float compressorApply(compressor_t *compressor, float input);
void compressorUpdate(compressor_t *compressor, float threshold_dB, float ratio);

uint8_t limiterApply(float *sample);

#endif
