#ifndef REVERB_H
#define REVERB_H

#define F_COMB_COUNT        4
#define F_ALLPASS_COUNT    3


typedef enum {
  F_COMB,
  F_ALLPASS
} filter_type_t;


typedef struct {
  filter_type_t type;
  float *buffer;
  uint32_t length;
  uint32_t pointer;
  float gain;
} filter_t;

typedef struct {
  filter_t filtersComb[F_COMB_COUNT];
  filter_t filtersAllPass[F_ALLPASS_COUNT];
} reverb_t;

void reverbInit(reverb_t *reverb);
float reverbApply(reverb_t *reverb, float sample);


#endif
