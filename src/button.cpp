#include "button.h"

Button::Button(uint8_t pin)
{
    bounce.attach(pin, INPUT_PULLUP);
}
void Button::update()
{
    press = None;
    bounce.update();
    if (bounce.fell())
    {
        fallen = true;
        fellTime = millis();
    }
    else if (bounce.rose())
    {
        if (fallen)
        {
            if (millis() - fellTime > 1000)
            {
                press = Long;
            }
            else
            {
                press = Short;
            }
        }
        fallen = false;
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