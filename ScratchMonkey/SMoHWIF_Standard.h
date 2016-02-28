// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoHWIF_Standard.h - Hardware interface for Arduino Uno etc.
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_STANDARD_
#define _SMO_HWIF_STANDARD_

//
// Debug Pin Assignment
//
// DEBUG    A4  Disabled for HVPP
//
typedef SMoHWIF_Debug_Soft<DEBUG_PIN(A4), true> SMoHWIF_Debug_Platform;

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
//      MOSI        11
//      MISO        12
//      SCK         13
//      XTAL         9          Optional Slave Clock
//
typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;

//
// High voltage protocols
//
//      Signal      Pin         Comment
//
//      HVRESET     10
//      SVCC        11
//
typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;

const int   SMoHWIF_PORT_B  = 0x03;
const int   SMoHWIF_PORT_C  = 0x06;
const int   SMoHWIF_PORT_D  = 0x09;

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
typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform, SMoHWIF_PORT_C,
    HVSP_SDI_BIT(0), HVSP_SII_BIT(1),
    HVSP_SDO_BIT(2), HVSP_SCI_BIT(3)>                       SMoHWIF_HVSP_Platform;

//
// TPI Pin Assignment (Signals are specified by bit position)
//
//      Signal      Pin         Comment
//
//      TPI_DATA    A2
//      TPI_CLK     A3
//
typedef SMoHWIF_TPI<SMoHWIF_HV_Platform, SMoHWIF_PORT_C,
                    TPI_DATA_BIT(2), TPI_CLK_BIT(3)>      SMoHWIF_TPI_Platform;

//
// HVPP Pin Assignment
//
// Control using port D (reusing the RX pin)
// Data split across ports B and C.
//
//      Signal      Pin         Comment
//      CTRL0        0          Arduino Uno (DIP MCU)
//      CTRL0       12          Arduino Pro Mini etc (SMD MCU)
//      CTRL2        2
//      CTRL3        3
//      CTRL4        4
//      CTRL5        5
//      CTRL6        6
//      CTRL7        7
//      DATA0       A0
//      DATA1       A1
//      DATA2       A2
//      DATA3       A3
//      DATA4       A4
//      DATA5       A5
//      DATA6        8
//      DATA7        9
//      RDY         12          Arduino Uno (DIP MCU)
//      RDY         A7          Arduino Pro Mini etc (SMD MCU)
//      XTAL        13
//

typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_B, 0x03, 6,
                          SMoHWIF_PORT_C, 0x3F, 0>          SMoHWIF_HVPP_Data;

#if NUM_ANALOG_INPUTS==8
//
// Arduino Pro Mini et al, SMD MCU, 8 analog inputs
//
typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_D, 0xFC, 0,
                          SMoHWIF_PORT_B, 0x10, 4>          SMoHWIF_HVPP_Control;
typedef SMoHWIF_Input_Pin_Analog<7>                         SMoHWIF_HVPP_Ready;

#else
//
// Arduino Uno, usually DIP MCU, 6 analog inputs
//
#define SMO_SHARE_SERIAL_PINS

typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_D>                 SMoHWIF_HVPP_Control;
typedef SMoHWIF_Input_Pin_Digital<12>                       SMoHWIF_HVPP_Ready;
#endif

typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
            SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
            SMoHWIF_HVPP_Ready>                             SMoHWIF_HVPP_Platform;

#endif /* _SMO_HWIF_STANDARD_ */
