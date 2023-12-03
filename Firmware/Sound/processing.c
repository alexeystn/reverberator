#include "main.h"
#include "defines.h"
#include "generator.h"
#include "reverb.h"
#include "filters.h"
#include "compressor.h"
#include "parameters.h"
#include "i2s_transfer.h"



float tableLevels[16] = {
    0.000, 0.026, 0.056, 0.089, 0.126, 0.168, 0.215, 0.267,
    0.326, 0.393, 0.467, 0.550, 0.644, 0.749, 0.867, 1.000,
};

float tableReverbGains[16][F_COMB_COUNT] = {
    { 0.451, 0.498, 0.408, 0.372 },
    { 0.512, 0.557, 0.470, 0.436 },
    { 0.570, 0.611, 0.530, 0.497 },
    { 0.623, 0.661, 0.586, 0.556 },
    { 0.672, 0.706, 0.638, 0.610 },
    { 0.716, 0.746, 0.686, 0.660 },
    { 0.755, 0.782, 0.728, 0.705 },
    { 0.789, 0.813, 0.766, 0.746 },
    { 0.820, 0.840, 0.799, 0.781 },
    { 0.846, 0.864, 0.828, 0.813 },
    { 0.869, 0.884, 0.853, 0.840 },
    { 0.888, 0.902, 0.875, 0.864 },
    { 0.905, 0.917, 0.894, 0.884 },
    { 0.920, 0.929, 0.910, 0.901 },
    { 0.932, 0.940, 0.924, 0.916 },
    { 0.943, 0.950, 0.936, 0.929 },
};

float tableCutoffFreq[16] = {
     200,   366,   575,   839,  1171,  1589,  2116,  2780,
    3617,  4671,  5999,  7672,  9781, 12437, 15784, 20000
};

float tableThresholds[16] = {
    20000, 17416, 15113, 13062, 11235,  9607,  8156,  6864,
     5713,  4687,  3774,  2960,  2234,  1588,  1013,   500
};

float tableRatios[16] = {
    1.000, 0.905, 0.817, 0.738, 0.665, 0.598, 0.538, 0.483,
    0.434, 0.389, 0.349, 0.312, 0.280, 0.250, 0.224, 0.200
    //1.000, 1.105, 1.223, 1.356, 1.505, 1.672, 1.859, 2.069,
    //2.305, 2.571, 2.868, 3.202, 3.577, 3.998, 4.470, 5.000
};


float masterLevel = 1.0f;
float reverbLevel = 0.0f;


reverb_t reverb;
compressor_t compressor;
biquadFilter_t filterLowCut;
pt1Filter_t filterHighCut;

uint8_t flagCompressor = 0;
uint8_t flagOverload = 0;

uint16_t peakLevelInput = 0;
uint16_t peakLevelOutput = 0;


void Processing_PutPeak(uint16_t *current, int16_t new)
{
  if (new > *current) {
    *current = new;
  }
  if (-new > *current) {
    *current = -new;
  }
}


void Processing_GetPeaks(uint16_t *inPeak, uint16_t *outPeak)
{
  __disable_irq();
  *inPeak = peakLevelInput;
  peakLevelInput = 0;
  *outPeak = peakLevelOutput;
  peakLevelOutput = 0;
  __enable_irq();
}


uint8_t Processing_GetCompressorFlag(void)
{
  uint8_t ret;
  __disable_irq();
  ret = flagCompressor;
  flagCompressor = 0;
  __enable_irq();
  return ret;
}


uint8_t Processing_GetOverloadFlag(void)
{
  uint8_t ret;
  __disable_irq();
  ret = flagOverload;
  flagOverload = 0;
  __enable_irq();
  return ret;
}


void Processing_Start(void)
{
  reverbInit(&reverb);
  compressorInit(&compressor);
  pt1FilterInit(&filterHighCut, 300);
  biquadFilterInit(&filterLowCut, 80, FILTER_HPF);
  flagCompressor = 0;
  flagOverload = 0;
  I2S_Transfer_Start();
}


int16_t Processing_Apply(int16_t input)
{
  float sample = input;
  float sampleRev;

  if ((input > 30000) || (input < -30000)) {
    flagOverload = 1;
  }
  if (compressor.state != C_IDLE) {
    flagCompressor = 1;
  }
  Processing_PutPeak(&peakLevelInput, (int16_t)sample);

  sample = biquadFilterApply(&filterLowCut, sample);
  sample = compressorApply(&compressor, sample);

  sampleRev = pt1FilterApply(&filterHighCut, sample);
  sampleRev = reverbApply(&reverb, sampleRev);

  sample = sample * masterLevel + sampleRev * reverbLevel;

  Processing_PutPeak(&peakLevelOutput, (int16_t)sample);

  return sample;
}


void AdjustParameter(uint8_t param, uint8_t value)
{
  __disable_irq();
  switch (param) {
  case P_MASTER_LEVEL:
    masterLevel = tableLevels[value];
    break;
  case P_REVERB_LEVEL:
    reverbLevel = tableLevels[value];
    break;
  case P_REVERB_TIME:
    for (int i = 0; i < F_COMB_COUNT; i++) {
      reverb.filtersComb[i].gain = tableReverbGains[value][i];
    }
    break;
  case P_REVERB_TONE:
    pt1FilterGain(tableCutoffFreq[value]);
    break;

  case P_COMPRESSOR_THRESHOLD:
    compressor.threshold = tableThresholds[value];
    break;

  case P_COMPRESSOR_RATIO:
    compressor.gain_reduce = tableRatios[value];
    break;
  }
  __enable_irq();
}




