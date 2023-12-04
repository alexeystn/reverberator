#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "parameters.h"
#include <stdio.h>

const char* parameterNames[P_COUNT] = {
    "Master  ",
    "Reverb  ",
    "Time    ",
    "Tone    ",
    "Thresh  ",
    "Ratio   ",
    "Save    "
};
