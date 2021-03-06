#ifndef H_CONFIG
#define H_CONFIG

#include <Arduino.h>
#include <MIDI.h>
#include <Preferences.h>

#define BANKSNUM 3

struct Msg
{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
    uint8_t altData2;
    uint8_t options;
    midi::MidiType getCmd();
    uint8_t getChannel();
    bool isToggle();
    // bool sendMidi();
    // bool sendUsb1();
    //bool sendUsb2();
    //bool sendPc();
    uint8_t getStatus();
};

struct Switch
{
    char name[7];
    Msg msgs[5];
    uint8_t loops;
};

struct Bank
{
    Switch switches[6];
};

class Config
{
    Preferences preferences;
  public:
    Bank bank;
    uint8_t currentBank = 0;
    uint8_t currentSwitch = 0;
    void read();
    Bank *current();
    void nextBank();
    void prevBank();
    void readBank(uint8_t bankNo);
    void saveSwitch(byte* data);
    void saveCurrentBank();
};
#endif