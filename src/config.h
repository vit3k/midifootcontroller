#ifndef H_CONFIG
#define H_CONFIG

#include <Arduino.h>
#include <MIDI.h>

#define BANKSNUM 5

struct Msg
{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
    midi::MidiType getCmd();
    uint8_t getChannel();
};

struct Switch
{
    char name[6];
    Msg msgs[5];
};

struct Bank
{
    Switch switches[6];
};

class Config
{
  public:
    //Bank banks[BANKSNUM];
    Bank bank;
    uint8_t currentBank = 0;
    uint8_t currentSwitch = 0;
    void saveBank();
    void read();
    Bank *current();
    void nextBank();
    void prevBank();
    void readBank(uint8_t bankNo);
};
#endif