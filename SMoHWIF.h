// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0    - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF.h      - Programmer hardware interface
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_
#define _SMO_HWIF_

#include "SMoHWIF_ISP.h"
#include "SMoHWIF_HV.h"
#include "SMoHWIF_HVSP.h"
#include "SMoHWIF_HVPP.h"
#include "SMoConfig.h"

template <typename ISP_Platform, typename HVSP_Platform, typename HVPP_Platform> 
class SMoHWIF_Platform {
public:
    typedef ISP_Platform    ISP;
    typedef HVSP_Platform   HVSP;
    typedef HVPP_Platform   HVPP;
};

#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
    typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;
    typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(A0)>        SMoHWIF_HV_Platform;
    typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
    //
    // Delegate controls to auxiliary 74HC595 shift register, but
    // can transfer data pretty easily
    //
    const int   SMoHWIF_PORT_B  = 0x03;
    const int   SMoHWIF_PORT_D  = 0x09;
    typedef SMoHWIF_Port_SPI<PORT_CS_PIN(A1)>                   SMoHWIF_HVPP_Control;
    typedef SMoHWIF_Port_Dual<SMoHWIF_PORT_B, 0x03, 6,
                              SMoHWIF_PORT_D, 0xFC, 2>         SMoHWIF_HVPP_Data;
    typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
                         SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data,
                         HVPP_XTAL_PIN(A2)>                     SMoHWIF_HVPP_Platform;
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
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
                              SMoHWIF_PORT_F, 0xF1, 0>         SMoHWIF_HVPP_Control;
    typedef SMoHWIF_Port_Slow<2>                                SMoHWIF_HVPP_Data;
    typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
                SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data>        SMoHWIF_HVPP_Platform;
#else
    typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(11)>   SMoHWIF_ISP_Platform;
    typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
    typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
    //
    // Megas have lots of contiguous pins, so we just use two full ports.
    //
    const int   SMoHWIF_PORT_F  = 0x0F;
    const int   SMoHWIF_PORT_K  = 0x106;
    typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_F>                SMoHWIF_HVPP_Control;
    typedef SMoHWIF_Port_Simple<SMoHWIF_PORT_K>                SMoHWIF_HVPP_Data;
    typedef SMoHWIF_HVPP<SMoHWIF_HV_Platform,
                SMoHWIF_HVPP_Control, SMoHWIF_HVPP_Data>        SMoHWIF_HVPP_Platform;
#endif

typedef SMoHWIF_Platform<
    SMoHWIF_ISP_Platform,
    SMoHWIF_HVSP_Platform,
    SMoHWIF_HVPP_Platform
>   SMoHWIF;

#endif /* _SMO_HWIF_ */
