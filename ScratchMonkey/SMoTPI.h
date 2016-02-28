// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoTPI.h           - Tiny Programming Interface (For 6 pin MCUs)
//
// Copyright (c) 2014-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// To use ScratchMonkey for TPI programming, refer to the target connection
// diagrams at http://microtherion.github.io/ScratchMonkey/Connection.html
//
#ifndef _SMO_TPI_
#define _SMO_TPI_

namespace SMoTPI {
    void EnterProgmode();
    void LeaveProgmode();
    void Erase();
    void WriteMem();
    void ReadMem();
    void SetParam();
} // namespace SMoTPI

#endif /* _SMO_TPI_ */
