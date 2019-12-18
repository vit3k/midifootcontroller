#ifndef H_BUTTON
#define H_BUTTON

#include <Arduino.h>
#include <Bounce2.h>

enum Press
{
    None,
    Short,
    Long
};

class Button
{
  private:
    Bounce bounce;
    unsigned long fellTime;
    bool fallen;
    Press press;
    unsigned long roseTime;

  public:
    Button(uint8_t pin);
    Button(uint8_t pin, uint8_t mode);
    void update();
    bool pressed();
    bool longpressed();
    bool read();
    bool down();
    bool up();
    void reset();
};
#endif