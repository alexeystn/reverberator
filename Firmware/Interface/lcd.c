
#include "main.h"
#include "defines.h"
#include "lcd.h"
#include <stdlib.h>
#include <memory.h>



#define HD44780_IOcmd      0
#define HD44780_IOdata     1
#define HD44780_IOcmd4bit  2

#define HD44780_CLEARDISPLAY  0x01
#define HD44780_RETURNHOME  0x02
#define HD44780_ENTRYMODESET  0x04
#define HD44780_DISPLAYCONTROL 0x08
#define HD44780_CURDISPSHIFT 0x10

#define HD44780_FUNCTIONSET  0x20
#define HD44780_8BITMODE     0x10
#define HD44780_4BITMODE     0x00

#define HD44780_SETCGRAMADDR 0x40
#define HD44780_SETDDRAMADDR 0x80

// flags for entry mode set;
//static const uint8_t HD44780_ENTRYLEFT2RIGHT = 0x02;
//static const uint8_t HD44780_ENTRYAUTOSHIFT = 0x01;
// flags for display on/off control;
#define HD44780_DISPLAYON  0x04
//  static const uint8_t HD44780_DISPLAYOFF = 0x00;
#define HD44780_CURSORON 0x02
#define HD44780_CURSOROFF 0x00
#define HD44780_BLINKON 0x01
#define HD44780_BLINKOFF 0x00

// flags for cursor/display shift;
//static const uint8_t HD44780_DISPLAYMOVE = 0x08;
//static const uint8_t HD44780_CURSORMOVE = 0x00;
//static const uint8_t HD44780_MOVERIGHT = 0x04;
//static const uint8_t HD44780_MOVELEFT = 0x00;
// flags for function set;
#define HD44780_2LINE  0x08
//static const uint8_t HD44780_1LINE = 0x00;
//static const uint8_t HD44780_5x10DOTS = 0x04;
//static const uint8_t HD44780_5x8DOTS  = 0x00;


static void HC595_Send(uint8_t rs, uint8_t e, uint8_t data4bits);
static void iowrite(uint8_t type, uint8_t value);


static void command4bit(uint8_t value)
{
  iowrite(HD44780_IOcmd4bit, value);
}

static void command(uint8_t value)
{
  iowrite(HD44780_IOcmd, value);
}



void LCD_Init(void)
{
  //HC595_Send(0, 0, 0);
  HAL_Delay(200);
  command4bit(HD44780_FUNCTIONSET | HD44780_8BITMODE);
  HAL_Delay(5);
  command4bit(HD44780_FUNCTIONSET | HD44780_8BITMODE);
  HAL_Delay(1);
  command4bit(HD44780_FUNCTIONSET | HD44780_8BITMODE);
  HAL_Delay(1);
  command4bit(HD44780_FUNCTIONSET | HD44780_4BITMODE);
  HAL_Delay(1);


  command(HD44780_FUNCTIONSET | (HD44780_4BITMODE | HD44780_2LINE));
  command(HD44780_DISPLAYCONTROL | HD44780_DISPLAYON | HD44780_CURSOROFF | HD44780_BLINKOFF);

  command(HD44780_CLEARDISPLAY);
  command(HD44780_RETURNHOME);
}

void LCD_Print(const char *text)
{
  for (int i = 0; i < strlen(text); i++) {
    iowrite(HD44780_IOdata, text[i]);
  }
}

void LCD_Bar(uint8_t value) //, uint8_t flag)
{
  if (value > 14)
    value = 14;

  for (int i = 0; i < value; i++)
    iowrite(HD44780_IOdata, 0xFF);

  for (int i = 0; i < (14-value); i++)
    iowrite(HD44780_IOdata, ' ');
}


void LCD_Slider(uint8_t value)
{
#if 0
  for (int i = 0; i < value+1; i++)
      iowrite(HD44780_IOdata, 0xFF);
  for (int i = 0; i < 16-value; i++)
      iowrite(HD44780_IOdata, '-');
#else
  for (int i = 0; i < value; i++)
      iowrite(HD44780_IOdata, '=');
  iowrite(HD44780_IOdata, 0xFF);
  for (int i = 0; i < 16-value; i++)
      iowrite(HD44780_IOdata, ' ');
#endif

}


void LCD_Position(uint8_t value, uint8_t size)
{
  for (int i = 0; i < value; i++)
      iowrite(HD44780_IOdata, '-');

  iowrite(HD44780_IOdata, 'O');
  for (int i = value+1; i < size; i++)
      iowrite(HD44780_IOdata, '-');
}


void LCD_SetCursor(uint8_t row, uint8_t col)
{
  command(HD44780_SETDDRAMADDR | (row * 0x40 + col));
}

void LCD_Clear(void)
{
  command(HD44780_CLEARDISPLAY);
  command(HD44780_RETURNHOME);
}


// duinoWitchery / hd44780
//
// 74HC595
//
// SER   - Serial input           - 14 - P1 - A2
// RCLK  - Storage register clock - 12 - P2 - A3
// SRCLK - Shift register clock   - 11 - P3 - A4

#define DATA_HIGH   HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_SET)
#define DATA_LOW    HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_RESET)
#define LATCH_HIGH  HAL_GPIO_WritePin(P2_GPIO_Port, P2_Pin, GPIO_PIN_SET)
#define LATCH_LOW   HAL_GPIO_WritePin(P2_GPIO_Port, P2_Pin, GPIO_PIN_RESET)
#define CLOCK_HIGH  HAL_GPIO_WritePin(P3_GPIO_Port, P3_Pin, GPIO_PIN_SET)
#define CLOCK_LOW   HAL_GPIO_WritePin(P3_GPIO_Port, P3_Pin, GPIO_PIN_RESET)

#define DELAY       { for (volatile int i = 0; i < 100; i++) {} }

#define TICK        { DELAY; CLOCK_HIGH; DELAY; CLOCK_LOW; }

static void HC595_Send(uint8_t rs, uint8_t e, uint8_t data4bits)
{
  DATA_LOW;
  CLOCK_LOW;
  LATCH_LOW;
  TICK;
  for (int i = 0; i < 4; i++) {
    if (data4bits & 0x08) DATA_HIGH; else DATA_LOW;
    data4bits <<= 1;
    TICK;
  }
  if (e) DATA_HIGH; else DATA_LOW;
  TICK;
  if (rs) DATA_HIGH; else DATA_LOW;
  TICK;
  DATA_LOW;
  TICK;
  DELAY;
  LATCH_HIGH;
  DELAY;
  LATCH_LOW;
  DELAY;
}

static void iowrite(uint8_t type, uint8_t value)
{
  uint8_t rs;
  if (type == HD44780_IOdata) {
    rs = 1;
  } else {
    rs = 0;
  }
  HC595_Send(rs, 0, value>>4);
  HC595_Send(rs, 1, value>>4);
  HC595_Send(rs, 0, value>>4);
  if (type != HD44780_IOcmd4bit) {
    HC595_Send(rs, 0, value & 0x0F);
    HC595_Send(rs, 1, value & 0x0F);
    HC595_Send(rs, 0, value & 0x0F);
  }
}
