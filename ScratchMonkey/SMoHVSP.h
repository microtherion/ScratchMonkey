// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoHVSP.h          - High Voltage Serial Programming
//                            (for MCUs with fewer than 20 pins)
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// To use ScratchMonkey for HVSP programming, refer to the target connection
// diagrams at http://microtherion.github.io/ScratchMonkey/Connection.html
//

#ifndef _SMO_HVSP_
#define _SMO_HVSP_

namespace SMoHVSP {
    void EnterProgmode();
    void LeaveProgmode();
    void ChipErase();
    void ProgramFlash();
    void ReadFlash();
    void ProgramEEPROM();
    void ReadEEPROM();
    void ProgramFuse();
    void ReadFuse();
    void ProgramLock();
    void ReadLock();
    void ReadSignature();
    void ReadOscCal();
} // namespace SMoHVSP

#endif /* _SMO_HVSP_ */
