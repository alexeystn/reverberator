#include "main.h"
#include "defines.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "processing.h"


#define PEAK_BUFFER_LEN  SSD1306_WIDTH

uint8_t peak_buffer[PEAK_BUFFER_LEN] = { 0, };
uint8_t peak_buffer_pointer = 0;


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
  ssd1306_Init();
}

void Interface_Loop(void)
{
  peak_buffer[peak_buffer_pointer] = Logarithm(Peak_Level_Get());
  peak_buffer_pointer++;
  if (peak_buffer_pointer == PEAK_BUFFER_LEN) {
    peak_buffer_pointer = 0;
  }
  ssd1306_Fill(Black);
  for (int i = 0; i < PEAK_BUFFER_LEN; i++) {
    uint8_t x = peak_buffer[(peak_buffer_pointer + i) % PEAK_BUFFER_LEN];
    ssd1306_Line(i, SSD1306_HEIGHT/2-x, i, SSD1306_HEIGHT/2+x, White);
  }
  ssd1306_UpdateScreen();
}