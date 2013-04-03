// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoConfig.h        - Configuration options
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#ifndef _SMO_CONFIG_
#define _SMO_CONFIG_

#include "Arduino.h"

//
// We support a number of different pin layouts:
//  - Traditional Arduino: SPI on pins 10-13, pins 0/1 used for Serial (ATmega168/328)
//  - Leonardo/Micro:      SPI on dedicated pins, pins 0/1 available   (ATmega32u4)
//  - Mega:                SPI on pins 50-53, pins 0/1 used for Serial (ATmega1280/2560)
//

#define    SMO_LAYOUT_TRADITIONAL     17
#define    SMO_LAYOUT_LEONARDO        22
#define    SMO_LAYOUT_MEGA            55

#if defined(__AVR_ATmega32U4__)
#define SMO_LAYOUT  SMO_LAYOUT_LEONARDO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define SMO_LAYOUT  SMO_LAYOUT_MEGA
#elif MOSI == 11
#define SMO_LAYOUT  SMO_LAYOUT_TRADITIONAL
#else
#error Unknown Arduino platform, help me define the correct pin layout
#endif

//
// Define to open a serial port for debugging
//
#undef SMO_WANT_DEBUG

//
// Some pins used in multiple modules
//
enum {
    SMO_HVRESET    = 10,
#if SMO_LAYOUT==SMO_LAYOUT_TRADITIONAL
    SMO_SVCC       = A0,
    SMO_DEBUG      = A5
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
    SMO_SVCC       = 11,
    SMO_DEBUG      = A4
#else
    SMO_SVCC       = 11,
    SMO_DEBUG      = 18
#endif
};

#endif /* _SMO_CONFIG_ */

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
