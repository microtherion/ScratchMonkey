// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Leonardo.h - Hardware interface for Arduino Leonardo / Micro 
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_LEONARDO_
#define _SMO_HWIF_LEONARDO_

//
// Debug Pin Assignment
//
// DEBUG    A4  Always available
//
typedef SMoHWIF_Debug_Soft<DEBUG_PIN(A4), false> SMoHWIF_Debug_Platform;

//
// Status LED Assignment (PGM/ERR same pin as ArduinoISP)
//
//      Signal      Pin Color   Comment
//      RDY          5  Green   Programmer is ready
//      VFY          6  Yellow  Programmer is verifying data
//      PGM          7  Orange  Programmer is writing
//      ERR          8  Red     Error occurred while programming
//
typedef SMoHWIF_Status<
    STATUS_RDY_PIN(5),
    STATUS_PGM_PIN(7),
    STATUS_VFY_PIN(6),
    STATUS_ERR_PIN(8),
    true>                       SMoHWIF_Status_Platform;

//
// ISP Pin Assignment
//
//      Signal      Pin         Comment
//
//      RESET       10
//      MOSI        MOSI / MO
//      MISO        MISO / MI
//      SCK         SCK  / CK
//      XTAL         9          Optional slave clock
//
typedef SMoHWIF_ISP<ISP_RESET_PIN(10), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;

//
// High voltage protocols
//
//      Signal      Pin         Comment
//
//      HVRESET     10
//      SVCC        11
//
typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;

const int   SMoHWIF_PORT_D  = 0x09;
const int   SMoHWIF_PORT_F  = 0x0F;

//
// HVSP Pin Assignment (Signals are specified by bit position)
//
//      Signal      Pin         Comment
//
//      SDI         A0
//      SII         A1
//      SDO         A2
//      SCI         A3
//
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform, SMoHWIF_PORT_F,
    HVSP_SDI_BIT(7), HVSP_SII_BIT(6),
    HVSP_SDO_BIT(5), HVSP_SCI_BIT(4)>                       SMoHWIF_HVSP_Platform;

//
// Leonardos don't have 8 contiguous pins anywhere, so we split the 
// control signals across two ports. The data signals are not as 
// critical, so we just use digitalRead/Write (we'd have to split
// them across at least three ports).
//
typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_D, 0x0C, 0,
                          SMoHWIF_PORT_F, 0xF1, 0>          SMoHWIF_HVPP_Control;
typedef SMoHWIF_Port_Slow<2>                                SMoHWIF_HVPP_Data;
typedef SMoHWIF_Input_Pin_Digital<12>                       SMoHWIF_HVPP_Ready;
typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
            SMoHWIF_HVPP_Ready>                             SMoHWIF_HVPP_Platform;
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform>                    SMoHWIF_TPI_Platform;

#endif /* _SMO_HWIF_LEONARDO_ */

