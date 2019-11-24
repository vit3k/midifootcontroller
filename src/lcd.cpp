#include "lcd.h"

void LCD::begin()
{
    lcd.begin(20, 2);
    memset(&buffer[0], ' ', 20);
    memset(&buffer[1], ' ', 20);
    lcd.noCursor();
}

void LCD::print(const char* fmt, ...)
{
    changed = true;
    va_list args;
    va_start(args, fmt);
    auto len = vsprintf(&buffer[row][col], fmt, args);
    if (col + len < 20)
    {
        buffer[row][col+len] = ' ';
    }
    else
    {
        buffer[row][col+len] = '\0';
    }
    va_end(args);
}

void LCD::cursorAt(uint8_t row, uint8_t col)
{
    lcd.setCursor(col, row);
    this->row = row;
    this->col = col;
}

void LCD::draw()
{
    if (changed)
    {
        // Serial.println("changed. drawing");
        // Serial.println(buffer[0]);
        // Serial.println(buffer[1]);
        lcd.setCursor(0, 0);
        lcd.println(buffer[0]);
        lcd.println(buffer[1]);
        changed = false;
    }
}

void LCD::cursor(uint8_t row, uint8_t col)
{
    lcd.setCursor(row, col);
    lcd.cursor();
}
void LCD::noCursor()
{
    lcd.noCursor();
}

void LCD::blink()
{
    lcd.blink();
}
void LCD::noBlink()
{
    lcd.noBlink();
}