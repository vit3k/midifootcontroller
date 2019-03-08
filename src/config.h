#ifndef H_CONFIG
#define H_CONFIG

#include <Arduino.h>

#define BANKSNUM 10

struct Msg
{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
};

struct Switch
{
    char name[6];
    Msg msgs[3];
};

struct Bank
{
    Switch switches[6];
};

class Config
{
  public:
    Bank banks[BANKSNUM];
    int8_t currentBank = 0;
    void saveBank();
    void read();
    Bank *current();
    void nextBank();
    void prevBank();
};
#endif