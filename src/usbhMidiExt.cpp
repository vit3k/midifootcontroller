#include "usbhMidiExt.h"

byte USBH_MIDI_ext::Init(byte parent, byte port, bool lowspeed)
{
    auto rcode = USBH_MIDI::Init(parent, port, lowspeed);
    if (rcode == 0) {
        this->port = port;
    }
    return rcode;
}