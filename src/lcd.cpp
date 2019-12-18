#include "lcd.h"

void LCD::begin()
{
    lcd.begin(20, 2);
    memset(&buffer[0], ' ', 20);
    memset(&buffer[1], ' ', 20);
    buffer[0][20] = 0;
    buffer[1][20] = 0;

    lcd.noCursor();
    cursorVisible = false;
}

void LCD::print(const char* fmt, ...)
{
    changed = true;
    va_list args;
    va_start(args, fmt);
    //Serial.printf("Drawing at %d,%d\n", row, col);
    char buf[21];
    auto len = vsprintf(buf, fmt, args);
    memcpy(&(buffer[row][col]), buf, len);
    //lcd.printf(fmt, args);
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
        buffer[0][20] = 0;
        buffer[1][20] = 0;
        //Serial.println("changed. drawing");
        //Serial.println(buffer[0]);
        //Serial.println(buffer[1]);
        lcd.setCursor(0, 0);
        lcd.print(buffer[0]);
        lcd.setCursor(0, 1);
        lcd.print(buffer[1]);
        if (cursorVisible)
        {
            cursor(cursorRow, cursorCol);
        }
        changed = false;
    }
}

void LCD::cursor(uint8_t row, uint8_t col)
{
    lcd.setCursor(col, row);
    lcd.cursor();
    cursorRow = row;
    cursorCol = col;
    cursorVisible = true;
}
void LCD::noCursor()
{
    lcd.noCursor();
    cursorVisible = false;
}

void LCD::blink()
{
    lcd.blink();
}
void LCD::noBlink()
{
    lcd.noBlink();
}