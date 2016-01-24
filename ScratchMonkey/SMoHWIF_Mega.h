// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Mega.h - Hardware interface for Arduino Mega 2560
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_MEGA_
#define _SMO_HWIF_MEGA_

//
// Debug Pin Assignment
//
// DEBUG    TX1     Always available
//
typedef SMoHWIF_Debug_Hard<Serial1> SMoHWIF_Debug_Platform;

//
// Status LED Assignment 
// Currently this is the only platform under which status is
// displayed while HVPP programming.
//
//      Signal      Pin Color   Comment
//      RDY          2  Green   Programmer is ready
//      VFY          3  Yellow  Programmer is verifying data
//      PGM          4  Orange  Programmer is writing
//      ERR          5  Red     Error occurred while programming
//
typedef SMoHWIF_Status<
    STATUS_RDY_PIN(2),
    STATUS_PGM_PIN(4),
    STATUS_VFY_PIN(3),
    STATUS_ERR_PIN(5),
    false>                      SMoHWIF_Status_Platform;

//
// ISP Pin Assignment
//
//      Signal      Pin         Comment
//
//      RESET       53
//      MOSI        51
//      MISO        50
//      SCK         52
//      XTAL        11          Optional slave clock
//
typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(11)>   SMoHWIF_ISP_Platform;

//
// High voltage protocols
//
//      Signal      Pin         Comment
//
//      HVRESET     10
//      SVCC        11
//
typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;

const int   SMoHWIF_PORT_F  = 0x0F;
const int   SMoHWIF_PORT_H  = 0xE0;
const int   SMoHWIF_PORT_K  = 0xE6;

//
// HVSP Pin Assignment (Signals are specified by bit position)
//
//      Signal      Pin         Comment
//
//      SDI          6
//      SII          7
//      SDO          8
//      SCI          9
//
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform, SMoHWIF_PORT_H,
    HVSP_SDI_BIT(3), HVSP_SII_BIT(4),
    HVSP_SDO_BIT(5), HVSP_SCI_BIT(6)>                       SMoHWIF_HVSP_Platform;

//
// TPI Pin Assignment (Signals are specified by bit position)
//
//      Signal      Pin         Comment
//
//      TPI_DATA     8
//      TPI_CLK      9
//
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform, SMoHWIF_PORT_H,
            TPI_DATA_BIT(5), TPI_CLK_BIT(6)>                SMoHWIF_TPI_Platform;

//
// HVPP Pin Assignment
//
// Megas have lots of contiguous pins, so we just use two full ports.
//
//      Signal      Pin         Comment
//      CTRL0       A0
//      CTRL2       A2
//      CTRL3       A3
//      CTRL4       A4
//      CTRL5       A5
//      CTRL6       A6
//      CTRL7       A7
//      DATA0       A8
//      DATA1       A9
//      DATA2       A10
//      DATA3       A11
//      DATA4       A12
//      DATA5       A13
//      DATA6       A14
//      DATA7       A15
//      RDY         12
//      XTAL        13
//
typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_F>                 SMoHWIF_HVPP_Control;
typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_K>                 SMoHWIF_HVPP_Data;
typedef SMoHWIF_Input_Pin_Digital<12>                       SMoHWIF_HVPP_Ready;
typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
            SMoHWIF_HVPP_Ready>                             SMoHWIF_HVPP_Platform;

#endif /* _SMO_HWIF_MEGA_ */

