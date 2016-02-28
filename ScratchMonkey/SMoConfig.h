// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoConfig.h        - Configuration options
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#ifndef _SMO_CONFIG_
#define _SMO_CONFIG_

#include "Arduino.h"

//
// Define to open a serial port for debugging
//
#undef DEBUG_ISP
#undef DEBUG_HVSP
#undef DEBUG_HVPP
#undef DEBUG_TPI
#undef DEBUG_COMM

#if defined(DEBUG_ISP) || defined(DEBUG_HVSP) || defined(DEBUG_HVPP) || defined(DEBUG_COMM) || defined(DEBUG_TPI)
#define SMO_WANT_DEBUG
#endif

#endif /* _SMO_CONFIG_ */
