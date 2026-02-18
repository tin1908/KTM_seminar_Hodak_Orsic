#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f4xx_hal.h"

#define LCD_I2C_ADDR  0x27   // ako ne radi, probaj 0x3F

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(char *str);
void LCD_Printf(const char *fmt, ...);
void LCD_Backlight_Off(void);
void LCD_Backlight_On(void);

#endif
