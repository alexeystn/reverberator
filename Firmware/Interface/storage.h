#ifndef STORAGE_H
#define STORAGE_H

#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "parameters.h"
#include <stdio.h>

void Storage_Save(void);
uint8_t Storage_Load(void);

#endif
