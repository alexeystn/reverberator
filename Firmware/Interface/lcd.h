#ifndef LCD_H
#define LCD_H

#include "main.h"

void LCD_Init(void);
void LCD_Print(char *text);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Bar(uint8_t value);
void LCD_Clear(void);

#endif
