// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Status.h     - Programmer hardware interface for status display
//
// Copyright (c) 2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_STATUS_
#define _SMO_HWIF_STATUS_

enum STATUS_RDY_PIN {};
enum STATUS_PGM_PIN {};
enum STATUS_VFY_PIN {};
enum STATUS_ERR_PIN {};

template <
    STATUS_RDY_PIN STATUS_RDY,
    STATUS_PGM_PIN STATUS_PGM,
    STATUS_VFY_PIN STATUS_VFY,
    STATUS_ERR_PIN STATUS_ERR,
    bool           PIN_CONSTRAINED
> class SMoHWIF_Status {
public:
    static void Setup() {
        pinMode(STATUS_RDY, OUTPUT);
        pinMode(STATUS_PGM, OUTPUT);
        pinMode(STATUS_VFY, OUTPUT);
        pinMode(STATUS_ERR, OUTPUT);
        Set(0);
    }
    static void Stop() {
        pinMode(STATUS_RDY, INPUT);
        pinMode(STATUS_PGM, INPUT);
        pinMode(STATUS_VFY, INPUT);
        pinMode(STATUS_ERR, INPUT);
    }
    static void StopIfPinsAreNeeded() {
        if (PIN_CONSTRAINED) Stop();
    }
    static void Set(int value) {
        digitalWrite(STATUS_RDY, (value & SCRATCHMONKEY_RDY_LED) != 0);
        digitalWrite(STATUS_PGM, (value & SCRATCHMONKEY_PGM_LED) != 0);
        digitalWrite(STATUS_VFY, (value & SCRATCHMONKEY_VFY_LED) != 0);
        digitalWrite(STATUS_ERR, (value & SCRATCHMONKEY_ERR_LED) != 0);
    }
};

#endif /* _SMO_HWIF_STATUS_ */
