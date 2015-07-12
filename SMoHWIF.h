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
#include "SMoConfig.h"

template <typename ISP_Platform, typename HVSP_Platform> class SMoHWIF_Platform {
public:
    typedef ISP_Platform    ISP;
    typedef HVSP_Platform   HVSP;
};

#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
    typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;
    typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(A0)>        SMoHWIF_HV_Platform;
    typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
    typedef SMoHWIF_ISP<ISP_RESET_PIN(10), ISP_CLOCK_PIN(9)>    SMoHWIF_ISP_Platform;
    typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
    typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
#else
    typedef SMoHWIF_ISP<ISP_RESET_PIN(SS), ISP_CLOCK_PIN(11)>   SMoHWIF_ISP_Platform;
    typedef SMoHWIF_HV<HV_RESET_PIN(10), HV_VCC_PIN(11)>        SMoHWIF_HV_Platform;
    typedef SMoHWIF_HVSP<SMoHWIF_HV_Platform>                   SMoHWIF_HVSP_Platform;
#endif

typedef SMoHWIF_Platform<
    SMoHWIF_ISP_Platform,
    SMoHWIF_HVSP_Platform
>   SMoHWIF;

#endif /* _SMO_HWIF_ */
