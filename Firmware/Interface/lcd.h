#ifndef LCD_H
#define LCD_H

#include "main.h"

void LCD_Init(void);
void LCD_Print(const char *text);
void LCD_Char(char ch);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Bar(uint8_t value);
void LCD_Slider(uint8_t value);
void LCD_Position(uint8_t value, uint8_t size);
void LCD_Clear(void);

#endif
