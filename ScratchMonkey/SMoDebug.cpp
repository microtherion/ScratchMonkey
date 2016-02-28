// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoDebug.cpp        - Debug serial port
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#include <Arduino.h>

#include "SMoDebug.h"
#include "SMoHWIF.h"

#ifdef SMO_WANT_DEBUG

void SMoDebugInit()
{
	SMoHWIF::Debug::Setup();
}

Stream & SMoDebugPort()
{
    return SMoHWIF::Debug::Port();
}
#endif
