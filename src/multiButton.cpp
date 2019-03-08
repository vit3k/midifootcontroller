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
        }
        down--;
    }
    if (button2->up())
    {
        if (down == 2)
        {
            press = Short;
        }
        down--;
    }
}

bool MultiButton::pressed()
{
    return press == Short;
}