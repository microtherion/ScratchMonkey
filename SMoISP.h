// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoISP.h           - In-System Programming commands
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// To use ScratchMonkey for ISP programming, connect the following pins
// of your arduino:
//
// PIN          Non-Mega Arduino        Arduino Mega (1280 and 2560)
//
// slave reset:     10                      53 
// MOSI:            11                      51 
// MISO:            12                      50 
// SCK:             13                      52 
//
// On your target system, connect these pins, and power, to the following
// pins of the DIP packages (for other packages, consult the data sheet):
//
// PIN      Tiny13/X5   TinyX4      Tiny26/X61  TinyX313    Mega*8      Mega*4
//          (8 pin)     (14 pin)    (20 pin)    (20 pin)    (28 pin)    (40 pin)
//
// RESET     1 (PB5)     4 (PB3)    10 (PB7)     1 (PA2)     1 (PC6)     9
// MOSI      5 (PB0)     7 (PA6)     1 (PB0)    17 (PB5)    17 (PB3)     6 (PB5)
// MISO      6 (PB1)     8 (PA5)     2 (PB1)    18 (PB6)    18 (PB4)     7 (PB6)
// SCK       7 (PB2)     9 (PA4)     3 (PB2)    19 (PB7)    19 (PB5)     8 (PB7)
// GND       4          14           6          10          22          31
// VCC       8           1           5          20           7          10
//
// Optionally, the following pins on your Arduino can be equipped with diagnostic
// LEDs (with suitable resistors):
//
// PIN      Function    Discussion
//
// 8        Error       Lights up if programming failed
// 7        Programming Communicating with slave
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
//
// LICENSE
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this 
//    list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
