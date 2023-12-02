#include "main.h"
#include "defines.h"
#include "processing.h"
#include "interface.h"
#include "parameters.h"
#include "lcd.h"
#include "cmsis_os.h"
#include <stdio.h>


extern ADC_HandleTypeDef hadc1;
extern xQueueHandle keysQueueHandle;


char text[20];
uint8_t rx_key;

uint8_t parameterValue[P_COUNT-1];
uint8_t pagePointer = 0;
uint8_t refreshScreen = 1;
uint8_t statusMark = 0;
uint32_t markResetTime = 0;

extern const char* parameterNames[P_COUNT];


void Interface_DefaultTask(void)
{
  uint32_t counter = 0;

  LCD_Init();
  Processing_Start();
  for (int i = 0; i < P_COUNT-1; i++) {
    parameterValue[i] = 7;
  }


  while (1) {
    rx_key = 0xFF;
    //sprintf(text, "%3d %3d %3d", counter, rx_key);
    xQueueReceive(keysQueueHandle, &rx_key, 0);


    if ((statusMark) && (HAL_GetTick() > markResetTime)) {
      statusMark = 0;
      refreshScreen = 1;
    }

    if (Processing_GetOverloadFlag()) {
      statusMark = 1;
      refreshScreen = 1;
      markResetTime = HAL_GetTick() + 1000;
    } else {
      if (Processing_GetCompressorFlag()) {
        statusMark = 2;
        refreshScreen = 1;
        markResetTime = HAL_GetTick() + 500;
      }
    }



    if (rx_key != 0xFF) {
      refreshScreen = 1;
    }

    if (rx_key == KEY_LEFT) {
      if (pagePointer == 0)
        pagePointer = P_COUNT-2;
      else
        pagePointer--;
    }
    if (rx_key == KEY_RIGHT) {
      if (pagePointer == P_COUNT-2)
        pagePointer = 0;
      else
        pagePointer++;
    }
    if (rx_key == KEY_MINUS) {
      if (parameterValue[pagePointer] > 0)
        parameterValue[pagePointer]--;
    }
    if (rx_key == KEY_PLUS) {
      if (parameterValue[pagePointer] < 15)
        parameterValue[pagePointer]++;
    }

    if (refreshScreen) {

      LCD_SetCursor(0, 0);
      if (statusMark == 0) LCD_Print(" "); else
      if (statusMark == 1) LCD_Print("*"); else
      if (statusMark == 2) LCD_Print(".");

      LCD_SetCursor(0, 1);
      LCD_Print(parameterNames[pagePointer]);
      LCD_SetCursor(0, 9);
      LCD_Position(pagePointer, P_COUNT-1);
      LCD_Print("         ");
      LCD_SetCursor(1, 0);
      LCD_Slider(parameterValue[pagePointer]);

      refreshScreen = 0;
    }
    osDelay(10);
    counter++;
  }
}


#define MAX_ADC 3100

void Interface_KeyboardTask(void)
{
  uint16_t adcVal;
  uint8_t key;
  uint8_t keyPrev = KEY_MAX;
  uint32_t lastKeyTime = 0;
  uint8_t keyChanged = 0;

  while (1) {


    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adcVal = HAL_ADC_GetValue(&hadc1);

    key = (adcVal + MAX_ADC/8) / (MAX_ADC / 4);
    if (key & 1) key = KEY_MAX - key; // swap 3 and 1


    if (key != keyPrev) {
      keyChanged = 1;
      lastKeyTime = HAL_GetTick();
    }
    keyPrev = key;

    if (keyChanged == 1) {
      if (HAL_GetTick() - lastKeyTime > 5) {
        keyChanged = 0;
        if (key != KEY_MAX) {
          xQueueSend(keysQueueHandle, &key, 0);
        }
      }
    }
    osDelay(1);

  }
}

