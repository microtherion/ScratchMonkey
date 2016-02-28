// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoGeneral.h       - Protocol independent global commands
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#ifndef _SMO_GENERAL_
#define _SMO_GENERAL_

#include <inttypes.h>

namespace SMoGeneral {
    extern uint16_t gSCKDuration;
    extern uint32_t gAddress;
    extern uint8_t  gControlStack[];
    extern uint8_t  gXPROGMode;

    void    SignOn();
    void    SetParam();
    void    GetParam();
    void    LoadAddress();
    void    SetControlStack();
    void    SetXPROGMode();
} // namespace SMoGeneral

#endif /* _SMO_GENERAL_ */
