#include "main.h"
#include "defines.h"
#include "reverb.h"
#include "filters.h"
#include "compressor.h"
#include "parameters.h"
#include "i2s_transfer.h"


float tableLevels[16] = {
    0.0000, 0.0158, 0.0224, 0.0316, 0.0447, 0.0631, 0.0891, 0.1259,
    0.1778, 0.2512, 0.3548, 0.5012, 0.7079, 1.0000, 1.4125, 1.9953
};

char *tableLabels[16] = {
    "-inf", " -36", " -33", " -30", " -27", " -24", " -21", " -18",
    " -15", " -12", "  -9", "  -6", "  -3", "  +0", "  +3", "  +6"
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

float tableRatios[16] = {
    1.0, 1.4, 1.8, 2.2, 2.6, 3.0, 3.4, 3.8,
    4.2, 4.6, 5.0, 5.4, 5.8, 6.2, 6.6, 7.0
};


float inputLevel1 = 1.0f;
float inputLevel2 = 1.0f;
float dryLevel = 1.0f;
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
  compressorInit(&compressor, 5000, 3);
  pt1FilterInit(&filterHighCut, 300);
  biquadFilterInit(&filterLowCut, 80, FILTER_HPF);
  flagCompressor = 0;
  flagOverload = 0;
  I2S_Transfer_Start();
}


int16_t Processing_Apply(int16_t input1, int16_t input2)
{
  float sample = ((float)input1)*inputLevel1 + ((float)input2)*inputLevel2;
  float sampleRev;

  if ((input1 > 30000) || (input1 < -30000)) {
    flagOverload = 1;
  }
  if ((input2 > 30000) || (input2 < -30000)) {
    flagOverload = 1;
  }
  if (compressor.state != C_IDLE) {
    flagCompressor = 1;
  }
  Processing_PutPeak(&peakLevelInput, (int16_t)sample);

  sample = biquadFilterApply(&filterLowCut, sample);
  sample = compressorApply(&compressor, sample);

  sampleRev = pt1FilterApply(&filterHighCut, sample);
  sampleRev = pt1FilterApply(&filterHighCut, sampleRev);
  sampleRev = reverbApply(&reverb, sampleRev);

  sample = sample * dryLevel + sampleRev * reverbLevel;

  flagOverload = limiterApply(&sample);

  Processing_PutPeak(&peakLevelOutput, (int16_t)sample);

  return sample;
}


void AdjustParameter(uint8_t param, uint8_t value)
{
  __disable_irq();
  switch (param) {
  case P_INPUT_LEVEL_1:
    inputLevel1 = tableLevels[value];
    break;
  case P_INPUT_LEVEL_2:
    inputLevel2 = tableLevels[value];
    break;
  case P_DRY_SIGNAL_LEVEL:
    dryLevel = tableLevels[value];
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
    filterHighCut.k = pt1FilterGain(tableCutoffFreq[value]);
    break;
  case P_COMPRESSOR_THRESHOLD:
  case P_COMPRESSOR_RATIO:
    compressorUpdate(&compressor, tableLevels[value], tableRatios[value]);
    break;
  }
  __enable_irq();
}




