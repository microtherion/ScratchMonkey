// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoHWIF_Debug.h    - Programmer hardware interface debug serial port
//
// Copyright (c) 2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_DEBUG_
#define _SMO_HWIF_DEBUG_

#include "SoftwareSerial.h"

enum DEBUG_PIN {};

template <DEBUG_PIN DEBUGPIN, bool PIN_CONSTRAINED> class SMoHWIF_Debug_Soft {
    static SoftwareSerial   sPort;
public:
    static Stream & Port() { return sPort; }
    static void Setup() {
        if (!sPort.isListening())
            sPort.begin(19200);
    }
    static void Stop() {
        sPort.end();
    }
    static void StopIfPinsAreNeeded() {
        if (PIN_CONSTRAINED) Stop();
    }
};

template <DEBUG_PIN DEBUGPIN, bool PIN_CONSTRAINED>
SoftwareSerial SMoHWIF_Debug_Soft<DEBUGPIN, PIN_CONSTRAINED>::sPort(-1, DEBUGPIN);

template <HardwareSerial & STREAM> class SMoHWIF_Debug_Hard {
public:
    static Stream & Port() { return STREAM; }
    static void Setup() {
        STREAM.begin(19200);
    }
    static void Stop() {
        STREAM.end();
    }
    static void StopIfPinsAreNeeded() {
    }
};

#endif /* _SMO_HWIF_DEBUG_ */
