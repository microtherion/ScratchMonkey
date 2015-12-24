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
//  - Standard Arduino (ATmega168/328)
//  - Leonardo/Micro   (ATmega32u4)
//  - Mega             (ATmega1280/2560)
//

#define    SMO_LAYOUT_STANDARD     0
#define    SMO_LAYOUT_LEONARDO     1
#define    SMO_LAYOUT_MEGA         2

#if defined(__AVR_ATmega32U4__)
#define SMO_LAYOUT  SMO_LAYOUT_LEONARDO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define SMO_LAYOUT  SMO_LAYOUT_MEGA
#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__)
#define SMO_LAYOUT  SMO_LAYOUT_STANDARD
#else
#error Unknown Arduino platform, help me define the correct pin layout
#endif

#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
#define SMO_SHARE_SERIAL_PINS
#endif

//
// Define to open a serial port for debugging
//
#undef DEBUG_ISP
#undef DEBUG_HVSP
#undef DEBUG_HVPP
#undef DEBUG_TPI
#undef DEBUG_COMM

#if defined(DEBUG_ISP) || defined(DEBUG_HVSP) || defined(DEBUG_HVPP) || defined(DEBUG_COMM) || defined(DEBUG_TPI)
#define SMO_WANT_DEBUG
#endif

//
// Some pins used in multiple modules
//
enum {
#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
    SMO_DEBUG      = A1
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
    SMO_DEBUG      = A4
#else
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
