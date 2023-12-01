#include "main.h"
#include "defines.h"
#include "generator.h"
#include "reverb.h"
#include "biquad.h"
#include "compressor.h"
#include "i2s_transfer.h"


reverb_t reverb;
compressor_t compressor;
biquadFilter_t filterHighCut, filterLowCut;


void Processing_Start(void)
{
  reverbInit(&reverb);
  compressorInit(&compressor);
  biquadFilterInit(&filterHighCut, 3000, FILTER_LPF);
  biquadFilterInit(&filterLowCut, 200, FILTER_HPF);
  I2S_Transfer_Start();
}


int16_t Processing_Apply(int16_t input)
{
  float sample = input;
  float sampleRev;

  sample = biquadFilterApply(&filterLowCut, sample);
  sample = compressorApply(&compressor, sample);

  sampleRev = biquadFilterApply(&filterHighCut, sample);
  sampleRev = reverbApply(&reverb, sampleRev);

  return (sample + sampleRev);
}
