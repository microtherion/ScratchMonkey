// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programmer for Arduino
//
// File: SMoTPI.h           - Tiny Programming Interface (For 6 pin MCUs)
//
// Copyright (c) 2014 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// To use ScratchMonkey for TPI programming, connect the following pins
// of your Arduino 
//
// PIN          Standard        Leonardo/Micro  Mega (1280 and 2560)
//
// RESET            10             10              10 
// SVCC             A0             11              11
// TPIDATA          12             12              12 
// TPICLK           13             13              13 
//
// If your target MCU did not have the RSTDISBL fuse programmed, you can connect
// the RESET pin directly, otherwise use RESET to generate a 12V high voltage
// signal (like for the HVSP and HVPP protocols), and attach that signal to the 
// RESET pin of the target system.
//
// On your target system, connect these pins, and power, to the following
// pins of the SOT-23 packages (for other packages, consult the data sheet):
//
// PIN      Tiny4/5/9/10
//          (6 pin)   
//
// RESET     6 (PB3)  
// TPIDATA   1 (PB0)  
// TPICLK    3 (PB1)  
// GND       2        
// VCC       5          // Connect to SVCC
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
