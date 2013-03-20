// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoHVSP.cpp        - High Voltage Serial Programming
//                            (for MCUs with fewer than 20 pins)
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#include "SMoHVSP.h"
#include "SMoCommand.h"
#include "SMoGeneral.h"

#include <Arduino.h>

enum {
    VCC   = 8,
    RESET = 9,
    SCI   = 10,
    SDI   = 11,
    SII   = 12,
    SDO   = 13,
};

inline bool
HVSPBit(bool instrInBit, bool dataInBit)
{
    digitalWrite(SII, instrInBit);
    digitalWrite(SDI, dataInBit);
    digitalWrite(SCI, HIGH);
    digitalWrite(SCI, LOW);
    
    return digitalRead(SDO);
}

static uint8_t
HVSPTransfer(uint8_t instrIn, uint8_t dataIn)
{
    //
    // First bit, data out only
    //
    uint8_t dataOut = HVSPBit(0, 0);

    //
    // Next bits, data in/out
    //
    for (char i=0; i<7; ++i) {
        dataOut = (dataOut << 1) | HVSPBit((instrIn & 0x80) != 0, (dataIn & 0x80) != 0);
        instrIn <<= 1;
        dataIn  <<= 1;
    }
    //
    // Last data out bit
    //
    HVSPBit(instrIn & 0x80, dataIn & 0x80);
    
    //
    // Two stop bits
    //
    HVSPBit(0, 0);
    HVSPBit(0, 0);
}

static bool
HVSPPollWait(uint8_t pollTimeout)
{
    uint32_t target = millis()+pollTimeout;
    while (millis() != target)
        if (digitalRead(SDO)) 
            return true;
    SMoCommand::SendResponse(STATUS_RDY_BSY_TOUT);
    return false;
}

void
SMoHVSP::EnterProgmode()
{
    // const uint8_t   stabDelay   = SMoCommand::gBody[1];
    // const uint8_t   cmdexeDelay = SMoCommand::gBody[2];
    const uint8_t   syncCyles   = SMoCommand::gBody[3];
    // const uint8_t   latchCycles = SMoCommand::gBody[4];
    // const uint8_t   toggleVtg   = SMoCommand::gBody[5];
    const uint8_t   powoffDelay = SMoCommand::gBody[6];
    const uint8_t   resetDelay1 = SMoCommand::gBody[7];
    const uint8_t   resetDelay2 = SMoCommand::gBody[8];
    
    pinMode(VCC, OUTPUT);
    digitalWrite(VCC, LOW);
    digitalWrite(RESET, HIGH); // Set BEFORE pinMode, so we don't glitch LOW
    pinMode(RESET, OUTPUT);
    pinMode(SCI, OUTPUT);
    digitalWrite(SCI, LOW);
    pinMode(SDI, OUTPUT);
    digitalWrite(SDI, LOW);
    pinMode(SCI, OUTPUT);
    digitalWrite(SCI, LOW);
    pinMode(SDO, OUTPUT);       // progEnable[2] on tinyX5
    digitalWrite(SDO, LOW);

    delay(powoffDelay);
    digitalWrite(VCC, HIGH);
    delayMicroseconds(40);
    for (uint8_t i=0; i<syncCycles; ++i) {
        digitalWrite(SCI, HIGH);
        digitalWrite(SCI, LOW);
    }
    digitalWrite(RESET, LOW);
    delayMicroseconds(20);
    pinMode(SDO, INPUT);
    
    SMoCommand::SendResponse();
}

void
SMoHVSP::LeaveProgmode()
{
    digitalWrite(RESET, HIGH);
    digitalWrite(VCC, LOW);

    SMoCommand::SendResponse();
}

void
SMoHVSP::ChipErase()
{
    const uint8_t pollTimeout   = SMoCommand::gBody[1];
    const uint8_t eraseTime     = SMoCommand::gBody[2];

    HVSPTransfer(0x4C, 0x80);
    HVSPTransfer(0x64, 0x00);
    HVSPTransfer(0x6C, 0x00);

    if (pollTimeout) {
        if (!HVSPPollWait(pollTimeout))
            return;
    } else {
        delay(eraseTime);
    }
    SMoCommand::SendResponse();
}

void
SMoHVSP::ProgramFlash()
{
    int16_t        numBytes     =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    const uint8_t   mode        =   SMoCommand::gBody[3];
    const uint8_t   pollTimeout =   SMoCommand::gBody[4];
    const uint8_t * data        =  &SMoCommand::gBody[5];

    //
    // Enter Flash Programming Mode
    //
    HVSPTransfer(0x4C, 0x10);
    bool timeout = false;
    if (mode & 1) { // Paged mode
        uint32_t address = SMoGeneral::gAddress;
        for (; numBytes > 0; numBytes -= 2) {
            //
            // Load Flash Page Buffer
            //
            HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
            HVSPTransfer(0x2C, *data++);
            HVSPTransfer(0x6D, 0x00);
            HVSPTransfer(0x6C, 0x00);
            HVSPTransfer(0x3C, *data++);
            HVSPTransfer(0x7D, 0x00);
            HVSPTransfer(0x7C, 0x00);
            ++SMoGeneral::gAddress;
        }
        if (mode & 0x80) { // Write page to flash
            //
            // Load Flash High Address and Program Page
            //
            HVSPTransfer(0x1C, address >> 8);
            HVSPTransfer(0x64, 0x00);
            HVSPTransfer(0x6C, 0x00);
            timeout = !HVSPPollWait(pollTimeout);
        }
    } else { // Word mode, ATtiny11/12
        uint32_t address = SMoGeneral::gAddress;
        HVSPTransfer(0x1C, address >> 8);
        for (; numBytes > 0; numBytes -= 2) {
            //
            // Write Flash Low / High Byte
            //
            HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
            HVSPTransfer(0x2C, *data++);
            HVSPTransfer(0x64, 0x00);
            HVSPTransfer(0x6C, 0x00);
            HVSPTransfer(0x3C, *data++);
            HVSPTransfer(0x74, 0x00);
            HVSPTransfer(0x7C, 0x00);
            ++SMoGeneral::gAddress;
            if ((timeout = !HVSPPollWait(pollTimeout)))
                break;
        }
    }
    //
    // Leave Flash Programming Mode
    //
    HVSPTransfer(0x4C, 0x00);
    if (!timeout)
        SMoCommand::SendResponse();
}

void
SMoHVSP::ReadFlash()
{
    int16_t     numBytes    =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    uint8_t *   outData     =  &SMoCommand::gBody[2];
    
    //
    // Flash Read
    //
    HVSPTransfer(0x4C, 0x02);
    for (; numBytes>0; numBytes-=2) {
        //
        // Read Flash Low and High Bytes
        //
        HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
        HVSPTransfer(0x1C, SMoGeneral::gAddress >> 8);
        HVSPTransfer(0x68, 0x00);
        *outData++ = HVSPTransfer(0x6C, 0x00);
        HVSPTransfer(0x78, 0x00);
        *outData++ = HVSPTransfer(0x7C, 0x00);
    }
    *outData = STATUS_CMD_OK;
    SMoCommand::SendResponse(STATUS_CMD_OK, outData-&SMoCommand::gBody[0]);
}

void
SMoHVSP::ProgramEEPROM()
{
    int16_t         numBytes     =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    const uint8_t   mode        =   SMoCommand::gBody[3];
    const uint8_t   pollTimeout =   SMoCommand::gBody[4];
    const uint8_t * data        =  &SMoCommand::gBody[5];

    //
    // Enter EEPROM Programming Mode
    //
    HVSPTransfer(0x4C, 0x11);
    bool timeout = false;
    if (mode & 1) { // Paged mode
        for (; numBytes > 0; --numBytes) {
            //
            // Load EEPROM Page Buffer
            //
            HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
            HVSPTransfer(0x1C, SMoGeneral::gAddress >> 8);
            HVSPTransfer(0x2C, *data++);
            HVSPTransfer(0x6D, 0x00);
            HVSPTransfer(0x6C, 0x00);
            ++SMoGeneral::gAddress;
        }
        if (mode & 0x80) { // Write page to EEPROM
            //
            // Load Program EEPROM Page
            //
            HVSPTransfer(0x64, 0x00);
            HVSPTransfer(0x6C, 0x00);
            timeout = !HVSPPollWait(pollTimeout);
        }
    } else { // Byte mode (ATtiny12)
        for (; numBytes > 0; --numBytes) {
            //
            // Load EEPROM Page Buffer
            //
            HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
            HVSPTransfer(0x2C, *data++);
            HVSPTransfer(0x64, 0x00);
            HVSPTransfer(0x6C, 0x00);
            ++SMoGeneral::gAddress;
             if ((timeout = !HVSPPollWait(pollTimeout)))
                break;
        }
    }
    //
    // Leave EEPROM Programming Mode
    //
    HVSPTransfer(0x4C, 0x00);
    if (!timeout)
        SMoCommand::SendResponse();    
}

void
SMoHVSP::ReadEEPROM()
{
    int16_t     numBytes    =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    uint8_t *   outData     =  &SMoCommand::gBody[2];
    
    //
    // EEPROM Read
    //
    HVSPTransfer(0x4C, 0x03);
    for (; numBytes>0; --numBytes) {
        //
        // Read EEPROM Byte
        //
        HVSPTransfer(0x0C, SMoGeneral::gAddress & 0xFF);
        HVSPTransfer(0x1C, SMoGeneral::gAddress >> 8);
        HVSPTransfer(0x68, 0x00);
        *outData++ = HVSPTransfer(0x6C, 0x00);
    }
    *outData = STATUS_CMD_OK;
    SMoCommand::SendResponse(STATUS_CMD_OK, outData-&SMoCommand::gBody[0]);
}

static void 
ProgramFuseLock(uint8_t d0, uint8_t i2, uint8_t i3)
{
    const uint8_t   value       = SMoCommand::gBody[2];
    const uint8_t   pollTimeout = SMoCommand::gBody[3];
    
    HVSPTransfer(0x4C, d0);
    HVSPTransfer(0x2C, value);
    HVSPTransfer(i2, 0x00);
    HVSPTransfer(i3, 0x00);

    if (HVSPPollWait(pollTimeout))
        SMoCommand::SendResponse();
}

static void
ReadFuseLock(uint8_t i1, uint8_t i2)
{
    uint8_t * dataOut   = &SMoCommand::gBody[2];

    HVSPTransfer(0x4C, 0x04);
    HVSPTransfer(i1, 0x00);
    *dataOut = HVSPTransfer(i2, 0x00);

    SMoCommand::SendResponse(STATUS_CMD_OK, 3);
}

void
SMoHVSP::ProgramFuse()
{
    const uint8_t   addr    = SMoCommand::gBody[1];
    const uint8_t   i2[3]   = {0x64, 0x74, 0x66};
    const uint8_t   i3[3]   = {0x6C, 0x7C, 0x6E};

    ProgramFuseLock(0x40, i2[addr], i3[addr]);
}

void
SMoHVSP::ReadFuse()
{
    const uint8_t   addr    = SMoCommand::gBody[1];
    const uint8_t   i1[3]   = {0x68, 0x7A, 0x6A};
    const uint8_t   i2[3]   = {0x6C, 0x7C, 0x6E};

    ReadFuseLock(i1[addr], i2[addr]);
}

void
SMoHVSP::ProgramLock()   
{
    ProgramFuseLock(0x20, 0x64, 0x6C);
}

void
SMoHVSP::ReadLock()       
{
    ReadFuseLock(0x78, 0x6C);
}

static void
ReadSignatureCal(uint8_t d1, uint8_t i2, uint8_t i3)
{
    uint8_t * dataOut   = &SMoCommand::gBody[2];

    HVSPTransfer(0x4C, 0x08);
    HVSPTransfer(0x0C, d1);
    HVSPTransfer(i2, 0x00);
    *dataOut = HVSPTransfer(i3, 0x00);

    SMoCommand::SendResponse(STATUS_CMD_OK, 3);
}

void
SMoHVSP::ReadSignature()  
{
    const uint8_t   addr    = SMoCommand::gBody[1];

    ReadSignatureCal(addr, 0x68, 0x6C);
}

void
SMoHVSP::ReadOscCal()     
{
    ReadSignatureCal(0x00, 0x78, 0x7C);
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
