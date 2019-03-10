#include "config.h"
#include <EEPROM.h>
#include "MIDI.h"

void Config::saveBank()
{
    EEPROM.put(1 + currentBank * sizeof(Bank), &bank);
}
void Config::read()
{
    currentBank = EEPROM.read(0);
    /*for(auto i = 0; i < BANKSNUM; i++)
    {
        Bank bank;
        for (auto j = 0; j < 6; j++)
        {
            memset(bank.switches[j].name, 0, 6);
            sprintf(bank.switches[j].name, "t%d %d", i+1, j+1);
            bank.switches[j].msgs[0].status = (1 << 4) | (i & 0x0F);
            bank.switches[j].msgs[0].data1 = j;
            bank.switches[j].msgs[0].data2 = 0;
            //memset(&bank + 6 + 3, 0, 4 * 3);
            for (auto k = 1; k < 5; k++)
            {
                bank.switches[j].msgs[k].status = 0;
                bank.switches[j].msgs[k].data1 = 0;
                bank.switches[j].msgs[k].data2 = 0;
            }
        }
        EEPROM.put(1 + i * sizeof(Bank), bank);
    }*/

    //EEPROM.get(1, banks);
    readBank(currentBank);
}
Bank *Config::current()
{
    //return &banks[currentBank];
    return &bank;
}
void Config::nextBank()
{
    currentBank++;
    if (currentBank >= BANKSNUM)
    {
        currentBank = 0;
    }
    readBank(currentBank);
    EEPROM.update(0, currentBank);
}
void Config::prevBank()
{
    currentBank--;
    if (currentBank < 0)
    {
        currentBank = BANKSNUM - 1;
    }
    readBank(currentBank);
    EEPROM.update(0, currentBank);
}
void Config::readBank(uint8_t bankNo)
{
    EEPROM.get(1 + bankNo * sizeof(Bank), bank);
}

midi::MidiType Msg::getCmd()
{
    uint8_t cmd = (status & 0xF0) >> 4;
    switch(cmd)
    {
        case 1:
            return midi::ProgramChange;
            break;
        case 2:
            return midi::ControlChange;
            break;
        default:
            return midi::InvalidType;
    }
}
uint8_t Msg::getChannel()
{
    return status & 0x0F;
}