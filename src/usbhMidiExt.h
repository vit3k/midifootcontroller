#ifndef H_USBMIDIEXT
#define H_USBMIDIEXT

#include <usbh_midi.h>

class USBH_MIDI_ext : public USBH_MIDI {
public:
    byte port = -1;
    byte Init(byte parent, byte port, bool lowspeed);
    USBH_MIDI_ext(USB* usb): USBH_MIDI(usb) {};
    virtual ~USBH_MIDI_ext() {}
    bool isActive() { return GetAddress() != 0; }
};

#endif