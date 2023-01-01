#ifndef FILTERS_H
#define FILTERS_H


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

#define F_COMB_COUNT 4
#define  F_ALLPASS_COUNT    3


void Filter_Init(filter_t *filter, filter_type_t type, float gain, float delay_ms);
float Filter_Process(filter_t *filter, float sample);


#endif
