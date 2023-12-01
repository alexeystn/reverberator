#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include <stdio.h>


#define PEAK_BUFFER_LEN  128

uint8_t peak_buffer[PEAK_BUFFER_LEN] = { 0, };
uint8_t peak_buffer_pointer = 0;
uint8_t dryLevel = 5, wetLevel = 5;


uint16_t log_scale[] = { 255, 300, 353, 415, 488, 574, 675, 794, 933, 1097, 1290, 1516,
    1782, 2095, 2463, 2896, 3404, 4002, 4705, 5531, 6501, 7643, 8985, 10562, 12416,
    14596, 17158, 20171, 23712, 27874, 32767 };


static uint8_t Logarithm(uint16_t value)
{
  uint8_t i;
  for (i = 0; i < sizeof(log_scale)/sizeof(log_scale[0]); i++) {
    if (log_scale[i] >= value) {
      return i;
    }
  }
  return 0;
}


void Interface_Init(void)
{
  LCD_Init();
}


uint8_t getKey(void)
{
  extern ADC_HandleTypeDef hadc1;
  HAL_ADC_Start(&hadc1);
  uint16_t val = HAL_ADC_GetValue(&hadc1);
  #define MAX_ADC 3100
  static uint8_t keyPrev = 0xFF;
  uint8_t key = (val + MAX_ADC/8) / (MAX_ADC / 4);
  uint8_t ret = 0xFF;
  if ((key != keyPrev) && (keyPrev != 0xFF)){
    ret = key;
  }
  keyPrev = key;
  return ret;
}



void Interface_Loop(void)
{
  static uint32_t nextFadeTime = 0;
  static uint8_t peak = 0;
  uint32_t timestamp = HAL_GetTick();

  uint8_t newPeak = 0; //Logarithm(Peak_Level_Get()) / 4;
  uint8_t comp = 0; //Compressor_Get_Flag();
  if (newPeak >= peak) {
    peak = newPeak;
    nextFadeTime = timestamp + 50;
  }
  if (peak > 0) {
    if (timestamp > nextFadeTime) {
      nextFadeTime = timestamp + 50;
      peak--;
    }
  }

  uint8_t key = getKey();

  switch (key) {
  case 0:
    if (dryLevel > 0) dryLevel--;
    break;
  case 3:
    if (dryLevel < 10) dryLevel++;
    break;

  case 2:
    if (wetLevel > 0) wetLevel--;
    break;
  case 1:
    if (wetLevel < 10) wetLevel++;
    break;
  }

  LCD_SetCursor(0, 0);
  char text[20];
  sprintf(text, "Dry%3d%% Wet%3d%%", dryLevel*10, wetLevel*10);
  LCD_Print(text);
  //Set_Dry_Wet(dryLevel, wetLevel);

  LCD_SetCursor(1, 0);
  LCD_Bar(peak+1, comp);

}
