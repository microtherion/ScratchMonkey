// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Standard.h - Hardware interface for Arduino Uno etc.
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_STANDARD_
#define _SMO_HWIF_STANDARD_

typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;

typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;

//
// Control using port D (reusing the RX pin), data
// split across ports B and C.
//
const int   SMoHWIF_PORT_B  = 0x03;
const int   SMoHWIF_PORT_C  = 0x06;
const int   SMoHWIF_PORT_D  = 0x09;

typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_B, 0x03, 6,
                          SMoHWIF_PORT_C, 0x3F, 0>          SMoHWIF_HVPP_Data;
typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_D>                 SMoHWIF_HVPP_Control;

#if NUM_ANALOG_INPUTS==8
//
// Arduino Pro Mini et al, SMD MCU, 8 analog inputs
//
typedef SMoHWIF_Input_Pin_Analog<7>                         SMoHWIF_HVPP_Ready;
#else
//
// Arduino Uno, usually DIP MCU, 6 analog inputs
//
typedef SMoHWIF_Input_Pin_Digital<12>                       SMoHWIF_HVPP_Ready;
#endif

typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
            SMoHWIF_HVPP_Ready>                             SMoHWIF_HVPP_Platform;
            
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform>                    SMoHWIF_TPI_Platform;

#endif /* _SMO_HWIF_STANDARD_ */

