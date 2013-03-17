// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoCommand.cpp     - Command parser 
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#include <Arduino.h>

#include "SMoCommand.h"

const uint16_t  kHeaderSize         = 5;
const uint16_t  kMaxBodySize        = 275;  // STK500 hardware limit

static uint8_t  sSequenceNumber;
static uint16_t sNumBytesRead       = 0;
static uint16_t sNumBytesWanted     = 1;
static uint8_t  sCheckSum           = 0;

uint16_t SMoCommand::gSize;
uint8_t  SMoCommand::gBody[kMaxBodySize+1];

static enum {
    kIdleState,         // Waiting for MESSAGE_START
    kHeaderState,       // Reading message header
    kBodyState,         // Reading message body
    kCompleteState      // Message being processed
}               sState              = kIdleState;

static void
ResetToIdle()
{
    sState          = kIdleState;
    sNumBytesRead   = 0;
    sNumBytesWanted = 1;
    sCheckSum       = 0;
}
//
// Parse next command, return command code if command is fully read
// and checksum matches. Handles timeouts and checksum errors 
// autonomously.
//
int 
SMoCommand::GetNextCommand()
{
    if (!Serial.available() || sState == kCompleteState)
        return kIncomplete;
    sCheckSum ^= (gBody[sNumBytesRead++] = Serial.read());
    if (sNumBytesRead < sNumBytesWanted)
        return kIncomplete;
    switch (sState) {
    case kIdleState:
        if (gBody[0] != MESSAGE_START) {
        reportHeaderError:
            ResetToIdle();
            return kHeaderError;
        } else {                            // Start of message
            sState          = kHeaderState;
            sNumBytesWanted = kHeaderSize;

            return kIncomplete;
        }
    case kHeaderState:
        if (gBody[4] != TOKEN)
            goto reportHeaderError;
        sNumBytesWanted = (uint16_t(gBody[2])<<8) | gBody[3];
        if (sNumBytesWanted > kMaxBodySize)
            goto reportHeaderError;
        sState          = kBodyState;
        sSequenceNumber = gBody[1];
        sNumBytesRead   = 0;
        ++sNumBytesWanted;  // For checksum byte
        
        return kIncomplete;
    case kBodyState:
        if (sCheckSum) {
            gBody[0] = ANSWER_CKSUM_ERROR;
            SendResponse(ANSWER_CKSUM_ERROR);
            return kChecksumError;
        } else {
            sState  = kCompleteState;
            gSize   = sNumBytesRead-1;
            return gBody[0];   // Success!
        }
    default:
      return kIncomplete;
    }
}

void        
SMoCommand::SendResponse(uint8_t status, uint16_t bodySize)
{
    gBody[1] = status;
    sCheckSum   = MESSAGE_START ^ TOKEN ^ sSequenceNumber;
    Serial.write(MESSAGE_START);
    Serial.write(sSequenceNumber);
    sCheckSum  ^= bodySize >> 8;
    Serial.write(bodySize >> 8);
    sCheckSum  ^= bodySize & 0xFF;
    Serial.write(bodySize & 0xFF);
    Serial.write(TOKEN);

    for (uint16_t i=0; i<bodySize; ++i)
        sCheckSum   ^= gBody[i];

    Serial.write(&gBody[0], bodySize);
    Serial.write(sCheckSum);

    ResetToIdle();
}

//
// LICENSE
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this 
//    list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
