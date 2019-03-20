#include <Arduino.h>
#include <MIDI.h>
#include "lcd.h"
#include <usbh_midi.h>
#include <usbhub.h>

#include "button.h"
#include "config.h"
#include "multiButton.h"
#include "usbhMidiExt.h"

USB usb;
USBHub hub(&usb);
USBH_MIDI_ext usbMidi1(&usb);
USBH_MIDI_ext usbMidi2(&usb);

struct MidiSettings : public midi::DefaultSettings
{
    static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MidiSettings);

Config config;
Button buttons[6] = {Button(2), Button(3), Button(4), Button(5), Button(6), Button(7)};
MultiButton bankUpButton(&buttons[0], &buttons[1]);
MultiButton bankDownButton(&buttons[1], &buttons[2]);
LCD lcd;
bool verbose = false;

void sendSysex(byte* data, uint8_t size);
void onSysex(byte* data, unsigned int size);
void sendUsbMidi(byte port, uint8_t msg[3]);
void sendSwitch(Switch* sw);

void updateButtons();
void drawRun();
void run();
void nextBank();
void prevBank();

void setup()
{
    if (usb.Init() == -1) {
        while (1);
    }
    delay( 200 );
    MIDI.setHandleSystemExclusive(onSysex);
    MIDI.turnThruOff();
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
    lcd.begin();
    config.read();
    drawRun();
    lcd.draw();
}

void loop()
{
    usb.Task();
    updateButtons();

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
        run();
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

void sendUsbMidi(byte port, uint8_t msg[3])
{
    if (usb.getUsbTaskState() == USB_STATE_RUNNING)
    {
        if (usbMidi1.port == port && usbMidi1.isActive())
        {
            usbMidi1.SendData(msg);
        }
        if (usbMidi2.port == port && usbMidi2.isActive())
        {
            usbMidi2.SendData(msg);
        }
    }
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
                if (msgs[msgIdx].sendMidi())
                {
                    MIDI.send(msgs[msgIdx].getCmd(), msgs[msgIdx].data1, msgs[msgIdx].data2, msgs[msgIdx].getChannel() + 1);
                }
                if (msgs[msgIdx].sendUsb1())
                {
                    uint8_t msg[3] = {msgs[msgIdx].getStatus(), msgs[msgIdx].data1, msgs[msgIdx].data2};
                    sendUsbMidi(0, msg);
                }
                if (msgs[msgIdx].sendUsb2())
                {
                    uint8_t msg[3] = {msgs[msgIdx].getStatus(), msgs[msgIdx].data1, msgs[msgIdx].data2};
                    sendUsbMidi(1, msg);
                }
            }
            config.currentSwitch = btnIdx;
            sendSwitch(&(bank->switches[btnIdx]));
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