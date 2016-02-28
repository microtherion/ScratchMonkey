// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoHWIF_HV.h      - Programmer hardware interface for high voltage protocols
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_HV_
#define _SMO_HWIF_HV_

enum HV_RESET_PIN {};
enum HV_VCC_PIN {};

template <HV_RESET_PIN HV_RESET, HV_VCC_PIN HV_VCC> class SMoHWIF_HV {
public:
    enum {
        RESET = HV_RESET,
        VCC   = HV_VCC
    };
};

#endif /* _SMO_HWIF_HV_ */
