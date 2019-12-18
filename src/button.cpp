#include "button.h"
#include <Arduino.h>

Button::Button(uint8_t pin)
{
    bounce.attach(pin, INPUT_PULLUP);
    bounce.interval(5);
}
Button::Button(uint8_t pin, uint8_t mode)
{
    bounce.attach(pin, mode);
    bounce.interval(5);
}
void Button::update()
{
    press = None;
    bounce.update();
    auto current = millis();
    if (bounce.fell())
    {
        fallen = true;
        fellTime = millis();
    }
    else if (bounce.rose())
    {
        if (fallen)
        {
            roseTime = current;
            fallen = false;
        }
    }
    if (fallen)
    {
        if (current - fellTime > 1000)
        {
            press = Long;
        }
    }
    if (roseTime != 0 && current - roseTime > 5)
    {
        if (roseTime - fellTime > 1000)
        {
            press = Long;
        }
        else
        {
            press = Short;
        }
        roseTime = 0;
    }
}
bool Button::pressed()
{
    return press == Short;
}
bool Button::longpressed()
{
    return press == Long;
}

bool Button::read()
{
    return bounce.read();
}

bool Button::down()
{
    return bounce.fell();
}

bool Button::up()
{
    return bounce.rose();
}

void Button::reset()
{
    roseTime = 0;
    fallen = false;
}