#include "multiButton.h"

void MultiButton::update()
{
    press = None;
    if (button1->down())
    {
        down++;
    }
    if (button2->down())
    {
        down++;
    }
    if (button1->up())
    {
        if (down == 2)
        {
            press = Short;
            button1->reset();
            button2->reset();
        }
        down = 0;

    }
    if (button2->up())
    {
        if (down == 2)
        {
            press = Short;
            button1->reset();

            button2->reset();
        }
        down = 0;
    }
}

bool MultiButton::pressed()
{
    return press == Short;
}