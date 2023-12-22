#ifndef INTERFACE_H
#define INTERFACE_H

#include "main.h"

typedef enum {
  KEY_LEFT,
  KEY_RIGHT,
  KEY_MINUS,
  KEY_PLUS,
  KEY_MAX
} keys_t;


typedef enum {
  MARK_OVERLOAD_IN = 0,
  MARK_OVERLOAD_OUT,
  MARK_COMPRESSOR,
  MARK_COUNT
} mark_t;


void Interface_DefaultTask(void);
void Interface_KeyboardTask(void);

#endif
