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
  enum comp_state_t state;
  int32_t attack;
  int32_t release;
  int32_t hold;
  int32_t timeout;
  float gain_reduce;
  float gain_step_attack;
  float gain_step_release;
  float gain;
  float threshold;
  uint8_t flag;
} compressor_t;


void compressorInit(compressor_t *compressor);
float compressorApply(compressor_t *compressor, float input);

#endif
