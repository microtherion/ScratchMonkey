// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Mega.h - Hardware interface for Arduino Mega 2560
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_MEGA_
#define _SMO_HWIF_MEGA_

typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(11)>   SMoHWIF_ISP_Platform;
typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
//
// Megas have lots of contiguous pins, so we just use two full ports.
//
const int   SMoHWIF_PORT_F  = 0x0F;
const int   SMoHWIF_PORT_K  = 0xE6;
typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_F>                SMoHWIF_HVPP_Control;
typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_K>                SMoHWIF_HVPP_Data;
typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data>        SMoHWIF_HVPP_Platform;
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform>                    SMoHWIF_TPI_Platform;

#endif /* _SMO_HWIF_MEGA_ */

