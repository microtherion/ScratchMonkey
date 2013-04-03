// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoHVPP.h          - High Voltage Parallel Programming
//                            (for MCUs with 20 pins and more)
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// To use ScratchMonkey for HVPP programming, connect the following pins
// of your arduino:
//
// PIN          Trad Arduino    Leonardo/Micro  Mega (1280 and 2560)
//
// D0-7           2-9             2-9          A8-A15                              
// HVRESET         10              10              10
// RDY             12              12              12
// SVCC            A0              11              11
// XTAL            A2              13              13
//
// MOSI 74595      11                                          
// SCK 74595       13                                           
// RCLK 74595      A1                       
//
// CTRL0                           A5              A0                              
// CTRL2                           RX (1)          A2
// CTRL3                           TX (0)          A3
// CTRL4                           A3              A4
// CTRL5                           A2              A5
// CTRL6                           A1              A6
// CTRL7                           A0              A7
//
// The HVRESET signal must then be used to generate a 12V signal to 
// apply to the RESET pin of the target MCU. A few suitable arrangements 
// are discussed in the manual, in the Generating High Voltage section.
//
// Since HVPP requires more signals than there are readily available pins
// on a Traditional Arduino, most of the signals are generated from a 74HC595
// shift register, programmed through the Arduino's SPI interface. Leonardo
// and Mega Arduinos, in contrast, have a sufficient number of direct pins.
//
// The tinyX61 family has a different mapping for the control pins, so
// their meaning is explained in the ALTMEANING column. All other MCUs 
// follow the pin meanings in MEANING, although the 20 pin tinys multiplex
// some of those pins.
//
// PIN      74HC595     MEANING     ALTMEANING
//
// MOSI  	14 (SER)
// RCLK  	12 (RCLK)
// SCK   	11 (SRCLK)
// CTRL0 	15 (QA)     BS2         -
// CTRL2 	 2 (QC)     OE          WR
// CTRL3 	 3 (QD)     WR          XA0
// CTRL4 	 4 (QE)     BS1         XA1
// CTRL5 	 5 (QF)     XA0         PAGEL
// CTRL6 	 6 (QG)     XA1         -
// CTRL7 	 7 (QH)     PAGEL       OE
// VCC   	16 (VCC)                            5V, NOT Slave VCC
//       	10 (SRCLR)
// GND   	 8 (GND)
//       	13 (OE)
//
// On your target system, connect these pins, and power, to the following
// pins of the DIP packages (for other packages, consult the data sheet):
//
// PIN      Tiny26/X61  TinyX313    Mega*8      Mega*4
//          (20 pin)    (20 pin)    (28 pin)    (40 pin)
//                                  
// RESET    10 (PB7)     1 (PA2)     1 (PC6)     9 (RESET)  From HV, NOT HVRESET
// XTAL      7 (PB4)     5 (PA0)     9 (PB6)    13 (XTAL1)
// RDY       9 (PB6)     3 (PD1)     3 (PD1)    15 (PD1)
// CTRL0     -           -          25 (PC2)    40 (PA0)
// CTRL2     1 (PB0)     6 (PD2)     4 (PD2)    16 (PD2)
// CTRL3     2 (PB1)     7 (PD3)     5 (PD3)    17 (PD3)
// CTRL4     3 (PB2)     8 (PD4)     6 (PD4)    18 (PD4)
// CTRL5     4 (PB3)     9 (PD5)    11 (PD5)    19 (PD5)
// CTRL6     -          11 (PD6)    12 (PD6)    20 (PD6)
// CTRL7     8 (PB5)     -          13 (PD7)    21 (PD7)
// DATA0    20 (PA0)    12 (PB0)    14 (PB0)     1 (PB0)
// DATA1    19 (PA1)    13 (PB1)    15 (PB1)     2 (PB1)
// DATA2    18 (PA2)    14 (PB2)    16 (PB2)     3 (PB2)
// DATA3    17 (PA3)    15 (PB3)    17 (PB3)     4 (PB3)
// DATA4    14 (PA4)    16 (PB4)    18 (PB4)     5 (PB4)
// DATA5    13 (PA5)    17 (PB5)    19 (PB5)     6 (PB5)
// DATA6    12 (PA6)    18 (PB6)    23 (PC0)     7 (PB6)
// DATA7    11 (PA6)    19 (PB7)    24 (PC1)     8 (PB7)
// SVCC      5 (VCC)    20 (VCC)     7 (VCC)    10 (VCC)    Slave VCC, NOT 12V
//          15 (AVCC)               20 (AVCC)   30 (AVCC)
// GND       6 (GND)    10 (GND)    22 (GND)    11 (GND)
//          16 (AGND)                           31 (GND)
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
