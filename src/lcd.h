#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>

class LCD {
private:
    LiquidCrystal lcd;
    char buffer[2][21];
    uint8_t row, col;
    bool changed = false;
public:
    LCD(): lcd(8, 9, 10, 11, 12, 13) {}
    void begin();
    void print(const char* fmt, ...);
    void cursorAt(uint8_t row, uint8_t col);
    void draw();
};
