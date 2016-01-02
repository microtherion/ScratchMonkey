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

inline void SMoDelay50ns() {
    __asm__ __volatile__("nop");
}

#include "SMoHWIF_ISP.h"
#include "SMoHWIF_HV.h"
#include "SMoHWIF_HVSP.h"
#include "SMoHWIF_HVPP.h"
#include "SMoHWIF_TPI.h"
#include "SMoConfig.h"

template <typename ISP_Platform, 
    typename HVSP_Platform, typename HVPP_Platform,
    typename TPI_Platform> 
class SMoHWIF_Platform {
public:
    typedef ISP_Platform    ISP;
    typedef HVSP_Platform   HVSP;
    typedef HVPP_Platform   HVPP;
    typedef TPI_Platform    TPI;
};

#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
#include "SMoHWIF_Standard.h"
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
#include "SMoHWIF_Leonardo.h"
#else
#include "SMoHWIF_Mega.h"
#endif

typedef SMoHWIF_Platform<
    SMoHWIF_ISP_Platform,
    SMoHWIF_HVSP_Platform,
    SMoHWIF_HVPP_Platform,
    SMoHWIF_TPI_Platform
>   SMoHWIF;

#endif /* _SMO_HWIF_ */
