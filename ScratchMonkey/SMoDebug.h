// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoDebug.h         - Debug serial port
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#ifndef _SMO_DEBUG_
#define _SMO_DEBUG_

#include "SMoConfig.h"

#ifdef SMO_WANT_DEBUG
void SMoDebugInit();
Stream & SMoDebugPort();
#define SMoDebug    SMoDebugPort()
#endif

#endif /* _SMO_DEBUG_ */
