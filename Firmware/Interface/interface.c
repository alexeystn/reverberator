#include "main.h"
#include "defines.h"
#include "processing.h"
#include "lcd.h"
#include "cmsis_os.h"
#include <stdio.h>


extern ADC_HandleTypeDef hadc1;
extern xQueueHandle keysQueueHandle;


char text[20];
uint8_t rx_key;


void Interface_DefaultTask(void)
{
  uint32_t counter = 0;

  LCD_Init();
  Processing_Start();

  while (1) {
    LCD_SetCursor(0, 0);
    xQueueReceive(keysQueueHandle, &rx_key, 0);
    sprintf(text, "%3d %3d %3d", counter, rx_key);
    rx_key = 9;
    LCD_Print(text);
    osDelay(300);
    counter++;
  }
}


#define MAX_ADC 3100
#define MAX_KEY 4

void Interface_KeyboardTask(void)
{
  uint16_t adcVal;
  uint8_t key;
  uint8_t keyPrev = MAX_KEY;
  uint32_t lastKeyTime = 0;
  uint8_t keyChanged = 0;

  while (1) {


    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adcVal = HAL_ADC_GetValue(&hadc1);

    key = (adcVal + MAX_ADC/8) / (MAX_ADC / 4);

    if (key != keyPrev) {
      keyChanged = 1;
      lastKeyTime = HAL_GetTick();
    }
    keyPrev = key;

    if (keyChanged == 1) {
      if (HAL_GetTick() - lastKeyTime > 5) {
        keyChanged = 0;
        if (key != MAX_KEY) {
          xQueueSend(keysQueueHandle, &key, 0);
        }
      }
    }
    osDelay(1);

  }
}

