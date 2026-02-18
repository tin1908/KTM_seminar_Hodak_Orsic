#include "lcd_i2c.h"
#include <stdarg.h>
#include <stdio.h>

static I2C_HandleTypeDef *lcd_i2c;

#define LCD_BACKLIGHT 0x08
#define EN 0x04
#define RS 0x01

static void lcd_write(uint8_t data)
{
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR << 1, &data, 1, HAL_MAX_DELAY);
}

static void lcd_pulse(uint8_t data)
{
    lcd_write(data | EN);
    HAL_Delay(1);
    lcd_write(data & ~EN);
    HAL_Delay(1);
}

static void lcd_send4(uint8_t data)
{
    lcd_write(data | LCD_BACKLIGHT);
    lcd_pulse(data | LCD_BACKLIGHT);
}

static void lcd_cmd(uint8_t cmd)
{
    lcd_send4(cmd & 0xF0);
    lcd_send4((cmd << 4) & 0xF0);
}

static void lcd_data(uint8_t data)
{
    lcd_send4((data & 0xF0) | RS);
    lcd_send4(((data << 4) & 0xF0) | RS);
}

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    lcd_send4(0x30);
    HAL_Delay(5);
    lcd_send4(0x30);
    HAL_Delay(5);
    lcd_send4(0x30);
    HAL_Delay(5);
    lcd_send4(0x20);   // 4-bit mode

    lcd_cmd(0x28); // 2 linije
    lcd_cmd(0x0C); // display ON
    lcd_cmd(0x06); // auto increment
    lcd_cmd(0x01); // clear
    HAL_Delay(2);
}

void LCD_Clear(void)
{
    lcd_cmd(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t row_addr[] = {0x00, 0x40};
    lcd_cmd(0x80 | (col + row_addr[row]));
}

void LCD_Print(char *str)
{
    while (*str)
        lcd_data(*str++);
}

void LCD_Printf(const char *fmt, ...)
{
    char buf[64];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    LCD_Print(buf);
}

void LCD_Backlight_Off(void)
{
    uint8_t data = 0x00;
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR << 1, &data, 1, HAL_MAX_DELAY);
}

void LCD_Backlight_On(void)
{
    uint8_t data = LCD_BACKLIGHT;
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR << 1, &data, 1, HAL_MAX_DELAY);
}
