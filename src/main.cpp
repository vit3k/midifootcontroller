#include <Arduino.h>
#include <MIDI.h>
#include "lcd.h"
#include "button.h"
#include "config.h"
#include "multiButton.h"
#include "rotaryEncoder.h"

struct MidiSettings : public midi::DefaultSettings
{
    static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MidiSettings);

Config config;
Button buttons[6] = {Button(6), Button(7), Button(8), Button(15), Button(2), Button(0)};
MultiButton bankUpButton(&buttons[0], &buttons[1]);
MultiButton bankDownButton(&buttons[1], &buttons[2]);
RotaryEncoder encoder1(4, 16);
//RotaryEncoder encoder2(11, 12);
Button encoder1button(17);
//Button encoder2button(13);

LCD lcd;
bool verbose = false;
byte editSwitch = -1;
byte editMsg = 0;
byte editCurrentPos = 0;
bool editCurrentActive = false;
Switch editCurrentSwitch;

void sendSysex(byte* data, uint8_t size);
void onSysex(byte* data, unsigned int size);
void sendSwitch(Switch* sw);

void updateButtons();
void drawRun();
void drawEdit();
void draw();
void run();
void edit();
void nextBank();
void prevBank();

void setup()
{
    MIDI.setHandleSystemExclusive(onSysex);
    MIDI.turnThruOff();
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
    lcd.begin();
    config.read();
    draw();
    lcd.draw();
}

void loop()
{
    updateButtons();
    if (editSwitch == -1)
    {
        run();
    }
    else
    {
        edit();
    }
    lcd.draw();
    MIDI.read();
}

void drawRun()
{
    auto bank = config.current();
    for (auto row = 0; row < 2; row++)
    {
        for (auto btnIdx = 0; btnIdx < 3; btnIdx++)
        {
            lcd.cursorAt(row, btnIdx * 6);
            lcd.print(bank->switches[row * 3 + btnIdx].name);
        }
    }
}

void drawEdit()
{
    auto msg = editCurrentSwitch.msgs[editMsg];
    lcd.cursorAt(0, 0);
    if (msg.getCmd() == midi::ProgramChange)
    {
        lcd.print("PC");
    }
    else if (msg.getCmd() == midi::ControlChange)
    {
        lcd.print("CC");
    }

    lcd.cursorAt(0, 3);
    lcd.print("%2d %3d %3d %3d", msg.getChannel(), msg.data1, msg.data2, msg.altData2);
    lcd.cursorAt(1, 0);
    lcd.print("%d/5 %s SV CN", editMsg, editCurrentSwitch.name);
    switch(editCurrentPos)
    {
        case 0:
            lcd.cursor(0, 0);
            break;
        case 1:
            lcd.cursor(0, 4);
            break;
        case 2:
            lcd.cursor(0, 8);
            break;
        case 3:
            lcd.cursor(0, 12);
            break;
        case 4:
            lcd.cursor(0, 16);
            break;
        case 5:
            lcd.cursor(1, 0);
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            lcd.cursor(1, editCurrentPos - 2);
            break;
        case 12:
            lcd.cursor(1, 12);
            break;
        case 13:
            lcd.cursor(1, 15);
            break;
    }
    if (editCurrentActive)
    {
        lcd.blink();
    }
    else
    {
        lcd.noBlink();
    }

}

void draw()
{
    if (editSwitch == -1)
    {
        drawRun();
    }
    else
    {
        drawEdit();
    }
}
void run()
{
    if (bankUpButton.pressed())
    {
        nextBank();
    }
    else if (bankDownButton.pressed())
    {
        prevBank();
    }
    else
    {
        auto bank = config.current();
        for (byte btnIdx = 0; btnIdx < 6; btnIdx++)
        {
            if (buttons[btnIdx].longpressed())
            {
                editSwitch = btnIdx;
                editMsg = 0;
                editCurrentPos = 0;
                editCurrentActive = false;
                memcpy(&editCurrentSwitch, &(bank->switches[btnIdx]), sizeof(Switch));
                draw();
            }
            else if (buttons[btnIdx].pressed())
            {
                draw();
                auto msgs = bank->switches[btnIdx].msgs;
                for (auto msgIdx = 0; msgIdx < 3; msgIdx++)
                {
                    MIDI.send(msgs[msgIdx].getCmd(), msgs[msgIdx].data1, msgs[msgIdx].data2, msgs[msgIdx].getChannel() + 1);
                }
                config.currentSwitch = btnIdx;
                sendSwitch(&(bank->switches[btnIdx]));
            }
        }
    }
}

void edit()
{
    if (encoder1button.pressed())
    {
        if (editCurrentPos == 13)
        {
            editSwitch = -1;
            draw();
            return;
        }
        else if (editCurrentPos == 12)
        {
            //TODO: save
            editSwitch = -1;
            draw();
            return;
        }
        else
        {
            editCurrentActive = !editCurrentActive;
            draw();
            return;
        }
    }

    if (!editCurrentActive)
    {
        if (encoder1.delta() > 0)
        {
            editCurrentPos++;
            if (editCurrentPos > 13)
            {
                editCurrentPos = 0;
            }
            draw();
        }
        else if (encoder1.delta() < 0)
        {
            editCurrentPos--;
            if (editCurrentPos < 0)
            {
                editCurrentPos = 13;
            }
            draw();
        }
    }
    else
    {
        if (encoder1.delta() != 0)
        {
            //TODO: change value
            draw();
        }
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

void nextBank()
{
    config.nextBank();
    drawRun();
    sendSwitch(&(config.current()->switches[config.currentSwitch]));
}

void prevBank()
{
    config.prevBank();
    drawRun();
    sendSwitch(&(config.current()->switches[config.currentSwitch]));
}

void sendSysex(byte* data, uint8_t size)
{
    byte* msg = new byte[3 + size];
    msg[0] = 0x66;
    msg[1] = 0x00;
    msg[2] = 0x00;
    memcpy(msg+3, data, size);
    MIDI.sendSysEx(3 + size, msg);
    delete msg;
}
void sendSwitch(Switch* sw)
{
    if (!verbose)
    {
        return;
    }
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
    verbose = true;
    switch(data[4])
    {
        case 0x01:
            sendSwitch(&(config.current()->switches[config.currentSwitch]));
            break;
        case 0x02:
            config.saveSwitch(data+5);
            break;
    }
}