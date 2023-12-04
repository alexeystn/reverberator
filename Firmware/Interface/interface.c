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
uint8_t pagePointer = P_REVERB_LEVEL;
uint8_t refreshScreen = 1;
uint8_t statusMark[3] = {0, 0, 0};
uint32_t markResetTime[3] = {0, 0, 0};
uint8_t peaks[2] = {0, 0};
uint32_t decayTime[2] = {0, 0};

#define SCREEN_MODE_MENU  0
#define SCREEN_MODE_BARS  1


uint32_t lastKeyPressTime = 0;
uint8_t screenMode = SCREEN_MODE_BARS;
uint8_t keyPressFlag = 0;

extern const char* parameterNames[P_COUNT];


uint8_t Logarithm(uint16_t val)
{
  uint8_t i;
  uint32_t x = 2;
  for (i = 0; i < 15; i++) {
    if (x > val) {
      break;
    }
    x *= 2;
  }
  return i;
}



uint8_t Update_Bars(void)
{
  uint8_t ret;
  uint16_t peakValues[2];
  uint8_t peakPoints[2];
  uint32_t time = HAL_GetTick();

  Processing_GetPeaks(&peakValues[0], &peakValues[1]);

  for (int i = 0; i < 2; i++) {
    if (peakValues[i] > 30000) {
      statusMark[i] = 1;
      ret = 1;
      markResetTime[i] = HAL_GetTick() + 1000;
    }

    peakPoints[i] = Logarithm(peakValues[i]);

    if (peakPoints[i] > peaks[i]) {
      peaks[i] = peakPoints[i];
      decayTime[i] = time + 50;
      ret = 1;
    }
    if (time > decayTime[i]) {
      if (peaks[i] > 0) {
        peaks[i]--;
        ret = 1;
      }
      decayTime[i] = time + 50;
    }
  }

  if (Processing_GetCompressorFlag()) {
    statusMark[2] = 1;
    ret = 1;
    markResetTime[2] = HAL_GetTick() + 1000;
  }

  for (int i = 0; i < 3; i++) {
    if ((statusMark[i]) && (HAL_GetTick() > markResetTime[i])) {
      statusMark[i] = 0;
    }
  }


  return ret;
}


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
    xQueueReceive(keysQueueHandle, &rx_key, 0);

    if (rx_key != 0xFF) {
      refreshScreen = 1;
      keyPressFlag = 1;
      lastKeyPressTime = HAL_GetTick();
    }

    if (screenMode == SCREEN_MODE_MENU) {
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
        if (parameterValue[pagePointer] > 0) {
          parameterValue[pagePointer]--;
          AdjustParameter(pagePointer, parameterValue[pagePointer]);
        }
      }
      if (rx_key == KEY_PLUS) {
        if (parameterValue[pagePointer] < 15) {
          parameterValue[pagePointer]++;
          AdjustParameter(pagePointer, parameterValue[pagePointer]);
        }
      }
      keyPressFlag = 0;
    }

    if (screenMode == SCREEN_MODE_BARS) {
      if (keyPressFlag) {
        screenMode = SCREEN_MODE_MENU;
        refreshScreen = 1;
        keyPressFlag = 0;
      }
    } else {  // SCREEN_MODE_MENU
      if ((HAL_GetTick() - lastKeyPressTime) > 2000) {
        screenMode = SCREEN_MODE_BARS;
        refreshScreen = 1;
      }
    }

    if (Update_Bars()) {
      refreshScreen = 1;
    }


    if (refreshScreen) {
      if (screenMode == SCREEN_MODE_BARS) {
        LCD_SetCursor(0, 0);
        LCD_Print("i");
        LCD_Bar(peaks[0]);
        LCD_SetCursor(1, 0);
        LCD_Print("o");
        LCD_Bar(peaks[1]);

        LCD_SetCursor(0, 15);
        if (statusMark[0]) {
          LCD_Print("*");
        } else if (statusMark[2]) {
          LCD_Print("C");
        } else {
          LCD_Print(" ");
        }
        LCD_SetCursor(1, 15);
        if (statusMark[1]) {
          LCD_Print("*");
        } else {
          LCD_Print(" ");
        }

      } else {  // SCREEN_MODE_MENU
        LCD_SetCursor(0, 0);
        LCD_SetCursor(0, 1);
        LCD_Print(parameterNames[pagePointer]);
        LCD_SetCursor(0, 9);
        LCD_Position(pagePointer, P_COUNT-1);
        LCD_Print("         ");
        LCD_SetCursor(1, 0);
        LCD_Slider(parameterValue[pagePointer]);
      }
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
  uint32_t lastEventTime = 0;
  uint8_t keyChanged = 0;
  uint32_t time;
  uint32_t nextRepeatTime = 0;

  while (1) {


    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    adcVal = HAL_ADC_GetValue(&hadc1);

    key = (adcVal + MAX_ADC/8) / (MAX_ADC / 4);
    if (key & 1) key = KEY_MAX - key; // swap 3 and 1

    time = HAL_GetTick();

    if (key != keyPrev) {
      if (key != KEY_MAX) {
        keyChanged = 1;
        lastKeyTime = time;
        nextRepeatTime = time + 500;
      }
    }

    if ((key == keyPrev) && (key != KEY_MAX)) {
      if (time > nextRepeatTime) {
        nextRepeatTime += 100;
        xQueueSend(keysQueueHandle, &key, 0);
      }
    }

    if (keyChanged == 1) {
      if ((time - lastKeyTime > 5) && (time - lastEventTime > 50)) {
        keyChanged = 0;
        if (key != KEY_MAX) {
          xQueueSend(keysQueueHandle, &key, 0);
          lastEventTime = time;
        }
      }
    }

    keyPrev = key;
    osDelay(1);

  }
}

