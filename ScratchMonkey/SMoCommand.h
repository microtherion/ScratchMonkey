// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoCommand.h       - Command parser 
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#ifndef _SMO_COMMAND_
#define _SMO_COMMAND_

#include "SMoHWIF.h"

#include <inttypes.h>

namespace SMoCommand {
    //
    // Special "command" codes
    //
    enum {
        kHeaderError    = -1,
        kChecksumError  = -2,
        kIncomplete     = 0
    };
    extern uint8_t  gBody[];
    extern uint16_t gSize;

    //
    // On regular Arduinos, we're low on pins for HVPP, so we need to share the serial
    // port with HVPP signals. This slows down processing, so we only turn it on when
    // needed. Based on an idea by João Paulo Barraca <jpbarraca@ua.pt>
    //
    void        ShareSerialPins(bool share);
    //
    // Parse next command, return command code if command is fully read
    // and checksum matches. Handles timeouts and checksum errors 
    // autonomously.
    //
    int         GetNextCommand();
    void        SendResponse(uint8_t status = STATUS_CMD_OK, uint16_t bodySize=2, bool xprog=false);
    void        SendXPROGResponse(uint8_t status = STATUS_CMD_OK, uint16_t bodySize=3);
} // namespace SMoCommand

#endif /* _SMO_COMMAND_ */
