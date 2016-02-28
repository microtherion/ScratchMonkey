// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoHVPP.h          - High Voltage Parallel Programming
//                            (for MCUs with 20 pins and more)
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// To use ScratchMonkey for HVPP programming, refer to the target connection
// diagrams at http://microtherion.github.io/ScratchMonkey/Connection.html
//

#ifndef _SMO_HVPP_
#define _SMO_HVPP_

namespace SMoHVPP {
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
} // namespace SMoHVPP

#endif /* _SMO_HVPP_ */
