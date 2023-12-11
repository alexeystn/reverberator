#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "parameters.h"
#include <stdio.h>

const char* parameterNames[P_COUNT] = {
    "Input 1   ",
    "Input 2   ",
    "Dry       ",
    "Reverb    ",
    "Reverb Time     ",
    "Reverb Tone     ",
    "Threshold ",
    "Ratio     ",
    "Save settings   "
};
