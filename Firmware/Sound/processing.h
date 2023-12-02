#ifndef PROCESSING_H
#define PROCESSING_H

#include "main.h"

void Processing_Start(void);
int16_t Processing_Apply(int16_t sample);
uint8_t Processing_GetCompressorFlag(void);
uint8_t Processing_GetOverloadFlag(void);

#endif
