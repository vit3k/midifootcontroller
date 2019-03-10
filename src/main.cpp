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
MultiButton bankDownButton(&buttons[1], &buttons[2]);

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
void sendSysex(byte* data, uint8_t size)
{
    byte* msg = new byte[3 + size];//{0x66, 0x00, 0x00};
    msg[0] = 0x66;
    msg[1] = 0x00;
    msg[2] = 0x00;
    memcpy(msg+3, data, size);
    MIDI.sendSysEx(3 + size, msg);
    delete msg;
}
void sendBank(Bank* bank)
{
    byte* msg = new byte[2 + sizeof(Bank)];
    msg[0] = 0x01;
    msg[1] = config.currentBank;
    memcpy(msg + 2, (uint8_t*)bank, sizeof(Bank));
    sendSysex(msg, 2 + sizeof(Bank));
    delete msg;
}
void sendSwitch(Switch* sw)
{
    byte* msg = new byte[3+ sizeof(Switch)];
    msg[0] = 0x01;
    msg[1] = config.currentBank;
    msg[2] = config.currentSwitch;
    memcpy(msg + 3, (uint8_t*)sw, sizeof(Switch));
    sendSysex(msg, 3 + sizeof(Switch));
    delete msg;
}
void onSysex(byte* data, unsigned int size)
{
    if (size < 5)
    {
        return;
    }
    if (data[0] != 0xF0 || data[1] != 0x66 || data[2] != 0x00)
    {
        return;
    }
    switch(data[4])
    {
        case 0x01:
            sendSwitch(&(config.current()->switches[config.currentSwitch]));
            break;
    }
}
void setup()
{
    MIDI.setHandleSystemExclusive(onSysex);
    MIDI.turnThruOn();
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
    for (byte btnIdx = 0; btnIdx < 6; btnIdx++)
    {
        if (buttons[btnIdx].pressed())
        {
            drawRun();
            auto msgs = bank->switches[btnIdx].msgs;
            for (auto msgIdx = 0; msgIdx < 3; msgIdx++)
            {
                MIDI.send(msgs[msgIdx].getCmd(), msgs[msgIdx].data1, msgs[msgIdx].data2, msgs[msgIdx].getChannel());
            }
            //byte msg[2] = {0x00, btnIdx};
            //sendSysex(msg, 2);
            config.currentSwitch = btnIdx;
            sendSwitch(&(bank->switches[btnIdx]));
        }
        if (buttons[btnIdx].longpressed())
        {
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
    bankDownButton.update();
}

void loop()
{

    updateButtons();

    if (bankUpButton.pressed())
    {
        //Serial.println("bank up");
        config.nextBank();
        drawRun();
        //sendBank(bank);
        sendSwitch(&(config.current()->switches[config.currentSwitch]));
    }
    else if (bankDownButton.pressed())
    {
        //Serial.println("bank down");
        config.prevBank();
        drawRun();
    }
    else if (currentState == Run)
    {
        run();
    }
    else if (currentState == Edit)
    {
        edit();
    }
    lcd.draw();
    MIDI.read();
}