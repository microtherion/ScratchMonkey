// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoHVSP.h          - High Voltage Serial Programming
//                            (for MCUs with fewer than 20 pins)
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// To use ScratchMonkey for HVSP programming, connect the following pins
// of your arduino:
//
// PIN          Trad Arduino    Leonardo/Micro  Mega (1280 and 2560)
//
// SVCC            A0              11              11
// HVRESET         10              10              10
// SDI              8               8               8
// SII              9               9               9
// SDO             12              12              12
// SCI             13              13              13
//
// The HVRESET signal must then be used to generate a 12V signal to 
// apply to the RESET pin of the target MCU. A few suitable arrangements 
// are discussed in the manual, in the Generating High Voltage section.
//
//
// On your target system, connect these pins, and power, to the following
// pins of the DIP packages (for other packages, consult the data sheet):
//
// PIN      Tiny13/X5   TinyX4      
//          (8 pin)     (14 pin)    
//
// RESET     1 (PB5)     4 (PB3)    From HV switch, NOT HVRESET
// SCI       2 (PB3)     2 (PB0)    
// SDI       5 (PB0)     7 (PA6)    
// SII       6 (PB1)     8 (PA5)    
// SDO       7 (PB2)     9 (PA4)
// VCC       8           1          SVCC, NOT 12V
// GND       4          14 
//                      13 (PA2)         
//                      12 (PA1)         
//                      13 (PA0)         
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
