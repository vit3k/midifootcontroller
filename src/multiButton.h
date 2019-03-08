#pragma once

#include "button.h"

class MultiButton
{
  private:
    Button* button1;
    Button* button2;
    Press press;
    uint8_t down;
  public:
    MultiButton(Button* button1, Button* button2): button1(button1), button2(button2) {}
    void update();
    bool pressed();
};