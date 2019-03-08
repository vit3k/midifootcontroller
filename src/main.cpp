#include <Arduino.h>
#include <MIDI.h>
#include "lcd.h"

#include "button.h"
#include "config.h"
#include "multiButton.h"

struct MidiSettings : public midi::DefaultSettings
{
    static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MidiSettings);

Button buttons[6] = {Button(2), Button(3), Button(4), Button(5), Button(6), Button(7)};
LCD lcd;
MultiButton bankUpButton(&buttons[0], &buttons[1]);

enum State
{
    Run,
    Edit
};
State currentState = Run;
uint8_t currentSwitchEdit = 0;
Config config;

void drawRun()
{
    auto bank = config.current();
    for (auto row = 0; row < 2; row++)
    {
        for (auto btnIdx = 0; btnIdx < 3; btnIdx++)
        {
            //Serial.println(bank->switches[row * 3 + btnIdx].name);
            lcd.cursorAt(row, btnIdx * 6);
            lcd.print(bank->switches[row * 3 + btnIdx].name);
        }
    }
}
void setup()
{
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
    lcd.begin();
    config.read();
    drawRun();
    lcd.draw();
}

void run()
{
    auto bank = config.current();
    for (auto btnIdx = 0; btnIdx < 6; btnIdx++)
    {
        if (buttons[btnIdx].pressed())
        {
            drawRun();
            //Serial.println(btnIdx);
            //Serial.println("pressed");
            auto msgs = bank->switches[btnIdx].msgs;
            for (auto msgIdx = 0; msgIdx < 3; msgIdx++)
            {
                MIDI.send((midi::MidiType)(msgs[msgIdx].status & 0xF0), msgs[msgIdx].data1, msgs[msgIdx].data2, msgs[msgIdx].status & 0x0F);
            }
        }
        if (buttons[btnIdx].longpressed())
        {
            Serial.println(btnIdx);
            Serial.println("long pressed");
            currentSwitchEdit = btnIdx;
            currentState = Edit;
        }
    }

}

void edit()
{
    if (buttons[0].pressed())
    {
        currentState = Run;
        config.saveBank();
    }
}

void updateButtons()
{
    for (auto btnIdx = 0; btnIdx < 6; btnIdx++)
    {
        buttons[btnIdx].update();
    }
    bankUpButton.update();
}

void loop()
{
    lcd.init();
    updateButtons();

    //Serial.println(buttons[0].read());
    if (bankUpButton.pressed())
    {
        Serial.println("bank up");
    }
    /*if (buttons[0].read() && buttons[1].read())
    {
        Serial.println("bank down");
        config.prevBank();
    }
    if (buttons[4].read() && buttons[5].read())
    {
        Serial.println("bank up");
        config.nextBank();
    }*/
    else if (currentState == Run)
    {
        run();
    }
    else if (currentState == Edit)
    {
        edit();
    }
    lcd.draw();
}