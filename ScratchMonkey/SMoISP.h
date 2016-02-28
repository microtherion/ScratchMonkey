// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoISP.h           - In-System Programming commands
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// To use ScratchMonkey for ISP programming, refer to the target connection
// diagrams at http://microtherion.github.io/ScratchMonkey/Connection.html
//
#ifndef _SMO_ISP_
#define _SMO_ISP_

namespace SMoISP {
    void EnterProgmode();
    void LeaveProgmode();
    void ChipErase();
    void ProgramFlash();
    void ReadFlash();
    void ProgramEEPROM();
    void ReadEEPROM();
    void ProgramFuse();
    void ReadFuse();
    inline void ProgramLock()      { ProgramFuse(); }
    inline void ReadLock()         { ReadFuse();    }
    inline void ReadSignature()    { ReadFuse();    }
    inline void ReadOscCal()       { ReadFuse();    }
    void SPIMulti();
} // namespace SMoISP

#endif /* _SMO_ISP_ */
