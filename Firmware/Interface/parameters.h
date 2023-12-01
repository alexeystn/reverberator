#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "main.h"

typedef enum {
  P_MASTER_LEVEL,
  P_REVERB_LEVEL,
  P_REVERB_TIME,
  P_REVERB_TONE,
  P_COMPRESSOR_THRESHOLD,
  P_COMPRESSOR_RATIO,
  P_SAVE_SETTINS,
  P_COUNT
} parameter_t;


const char* parameterNames[P_COUNT] = {
    "Master  ",
    "Reverb  ",
    "Time    ",
    "Tone    ",
    "Thresh  ",
    "Ratio   ",
    "Save settings"
};


#endif
