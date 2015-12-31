// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Leonardo.h - Hardware interface for Arduino Leonardo / Micro 
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_LEONARDO_
#define _SMO_HWIF_LEONARDO_

typedef SMoHWIF_ISP<ISP_RESET_PIN(10), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;
typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
//
// Leonardos don't have 8 contiguous pins anywhere, so we split the 
// control signals across two ports. The data signals are not as 
// critical, so we just use digitalRead/Write (we'd have to split
// them across at least three ports).
//
const int   SMoHWIF_PORT_D  = 0x09;
const int   SMoHWIF_PORT_F  = 0x0F;
typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_D, 0x0C, 0,
                          SMoHWIF_PORT_F, 0xF1, 0>          SMoHWIF_HVPP_Control;
typedef SMoHWIF_Port_Slow<2>                                SMoHWIF_HVPP_Data;
typedef SMoHWIF_Input_Pin_Digital<12>                       SMoHWIF_HVPP_Ready;
typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
            SMoHWIF_HVPP_Ready>                             SMoHWIF_HVPP_Platform;
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform>                    SMoHWIF_TPI_Platform;

#endif /* _SMO_HWIF_LEONARDO_ */

