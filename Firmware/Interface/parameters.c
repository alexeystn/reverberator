#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "parameters.h"
#include <stdio.h>

const char* parameterNames[P_COUNT] = {
    "Input 1 level   ",
    "Input 2 level   ",
    "Dry signal level",
    "Reverb Level    ",
    "Reverb Time     ",
    "Reverb Tone     ",
    "Threshold       ",
    "Compress Ratio  ",
    "Save settings   "
};
