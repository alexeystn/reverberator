#include "main.h"
#include "defines.h"
#include "generator.h"
#include "reverb.h"
#include "filters.h"
#include "compressor.h"
#include "i2s_transfer.h"


reverb_t reverb;
compressor_t compressor;
biquadFilter_t filterLowCut;
pt1Filter_t filterHighCut;

uint8_t flagCompressor = 0;
uint8_t flagOverload = 0;


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
  biquadFilterInit(&filterLowCut, 100, FILTER_HPF);
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

  sample = biquadFilterApply(&filterLowCut, sample);
  sample = compressorApply(&compressor, sample);

  sampleRev = pt1FilterApply(&filterHighCut, sample);
  sampleRev = reverbApply(&reverb, sampleRev);

  return sampleRev; //(sample + sampleRev);
}
