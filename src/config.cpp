#include "config.h"
#include <EEPROM.h>
#include "MIDI.h"

void Config::saveBank()
{
    EEPROM.put(1 + currentBank * sizeof(Bank), banks[currentBank]);
}
void Config::read()
{
    currentBank = EEPROM.read(0);
    for(auto i = 0; i < BANKSNUM; i++)
    {
        for (auto j = 0; j < 6; j++)
        {
            sprintf(banks[i].switches[j].name, "t%d %d", i+1, j+1);
            banks[i].switches[j].msgs[0].status = midi::ProgramChange | (i & 0x0F);
            banks[i].switches[j].msgs[0].data1 = j;
        }
    }
    //EEPROM.get(1, banks);
}
Bank *Config::current()
{
    return &banks[currentBank];
}
void Config::nextBank()
{
    currentBank++;
    if (currentBank >= BANKSNUM)
    {
        currentBank = 0;
    }
    EEPROM.update(0, currentBank);
}
void Config::prevBank()
{
    currentBank--;
    if (currentBank < 0)
    {
        currentBank = BANKSNUM - 1;
    }
    EEPROM.update(0, currentBank);
}