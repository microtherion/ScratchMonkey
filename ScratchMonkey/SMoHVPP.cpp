// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoHVPP.cpp        - High Voltage Parallel Programming
//                            (for MCUs with 20 pins and more)
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#include "SMoHVPP.h"
#include "SMoCommand.h"
#include "SMoGeneral.h"
#include "SMoConfig.h"
#include "SMoHWIF.h"

#ifdef DEBUG_HVPP
#include "SMoDebug.h"
#endif

#include <Arduino.h>
#include <SPI.h>

enum {
    HVPP_XTAL   = SMoHWIF::HVPP::XTAL
};

//
// HVPP, for 28 pins and more, requires 8 output signals and 1 input 
// signal. 20 pin MCUs multiplex some of the output signals, and they
// don't do it all the same way. We rely on the control stack uploaded
// by avrdude to tell us what signals to set, when. 
//
// Control patterns, expressed as index values for the control stack
// (Adopted from Simon Quiang's AVRminiProg implementation)
//
enum {
    kLoadAddr       =  0,
    kLoadData       =  4,
    kLoadCommand    =  8,
    kDone           = 12,
    kCommitData     = 16,
    kEnableRead     = 20,
    kPageLoad       = 24,
    kInit           = 28,

    kLowByte        = 0,
    kHighByte       = 1,
    kExtByte        = 2,
    kExt2Byte       = 3,
};

inline void
HVPPSetControls(uint8_t controlIx)
{
#ifdef DEBUG_HVPP
    SMoDebug.print("Ctrl ");
    SMoDebug.print(controlIx, DEC);
    SMoDebug.print(" / ");
    SMoDebug.print(SMoGeneral::gControlStack[controlIx], BIN);
    SMoDebug.println();
#endif
    SMoHWIF::HVPP::SetControlSignals(SMoGeneral::gControlStack[controlIx]);
    delayMicroseconds(2);
}

inline void
HVPPSetControls(uint8_t controlIx, uint8_t byteSel)
{
    HVPPSetControls(controlIx+byteSel);
}

inline void
HVPPDataMode(uint8_t mode)
{
#ifdef DEBUG_HVPP
   SMoDebug.println(mode == OUTPUT ? "Data OUT" : "Data IN");
#endif
   SMoHWIF::HVPP::DataMode(mode);
}

inline void
HVPPSetDataRaw(uint8_t dataOut)
{
#ifdef DEBUG_HVPP
   SMoDebug.print("Data<");
   SMoDebug.println(dataOut, HEX);
#endif
   SMoHWIF::HVPP::SetData(dataOut);
}

inline uint8_t
HVPPGetDataRaw()
{
    uint8_t dataIn = SMoHWIF::HVPP::GetData();
    
#ifdef DEBUG_HVPP
    SMoDebug.print("Data>");
    SMoDebug.println(dataIn, HEX);
#endif

    return dataIn;
}

inline void
HVPPWriteData(uint8_t controlIx, uint8_t dataOut)
{
    HVPPSetControls(controlIx);
    HVPPSetDataRaw(dataOut);
    digitalWrite(HVPP_XTAL, HIGH);
    digitalWrite(HVPP_XTAL, LOW);
}

inline void
HVPPWriteData(uint8_t controlIx, uint8_t byteSel, uint8_t dataOut)
{
    HVPPSetControls(controlIx, byteSel);
    HVPPSetDataRaw(dataOut);
    digitalWrite(HVPP_XTAL, HIGH);
    digitalWrite(HVPP_XTAL, LOW);
}

inline void
HVPPLoadCommand(uint8_t command)
{
    HVPPDataMode(OUTPUT);
    HVPPWriteData(kLoadCommand, command);
}

inline void
HVPPLoadAddress(uint8_t byteSel, uint8_t addr)
{
    HVPPWriteData(kLoadAddr, byteSel, addr);
}

inline void
HVPPLoadData(uint8_t byteSel, uint8_t addr)
{
    HVPPWriteData(kLoadData, byteSel, addr);
}

inline void
HVPPCommitData(uint8_t byteSel=kLowByte)
{
    HVPPSetControls(kDone, byteSel);
    HVPPSetControls(kCommitData, byteSel);
    HVPPSetControls(kDone, byteSel);
}

inline void
HVPPCommitDataWithPulseWidth(uint8_t pulseWidth, uint8_t byteSel=kLowByte)
{
    HVPPSetControls(kDone, byteSel);
    HVPPSetControls(kCommitData, byteSel);
    if (pulseWidth)
        delay(pulseWidth);
    HVPPSetControls(kDone, byteSel);
}

inline uint8_t
HVPPReadData(uint8_t byteSel)
{
    HVPPSetControls(kLoadData, byteSel);
    HVPPSetControls(kEnableRead, byteSel);
    return HVPPGetDataRaw();
}

static bool
HVPPPollWait(uint8_t pollTimeout)
{
    uint32_t target = millis()+pollTimeout+10;
    while (millis() != target)
        if (SMoHWIF::HVPP::GetReady()) 
            return true;
    SMoCommand::SendResponse(STATUS_RDY_BSY_TOUT);
    return false;
}

void
SMoHVPP::EnterProgmode()
{
#ifdef DEBUG_HVPP
    SMoDebugInit();
#endif

    // const uint8_t   stabDelay   = SMoCommand::gBody[1];
    // const uint8_t   cmdexeDelay = SMoCommand::gBody[2];
    const uint8_t   latchCycles = SMoCommand::gBody[3];
    // const uint8_t   toggleVtg   = SMoCommand::gBody[4];
    const uint8_t   powOffDelay = SMoCommand::gBody[5];
    const uint8_t   resetDelay1 = SMoCommand::gBody[6];
    const uint8_t   resetDelay2 = SMoCommand::gBody[7];
   
    SMoHWIF::HVPP::Setup(SMoGeneral::gControlStack[kInit], powOffDelay, latchCycles); 
    delay(resetDelay1);
    delayMicroseconds(resetDelay2);
    
    SMoCommand::SendResponse();
    
    HVPPSetControls(kDone);
}

void
SMoHVPP::LeaveProgmode()
{
    // const uint8_t   stabDelay   = SMoCommand::gBody[1];
    const uint8_t   resetDelay = SMoCommand::gBody[2];

    SMoHWIF::HVPP::Stop();

    delay(resetDelay);

    SMoCommand::SendResponse();
}

void
SMoHVPP::ChipErase()
{
    const uint8_t pulseWidth    = SMoCommand::gBody[1];
    const uint8_t pollTimeout   = SMoCommand::gBody[2];

    HVPPLoadCommand(0x80);
    HVPPCommitDataWithPulseWidth(pulseWidth);

    if (pollTimeout) {
        if (!HVPPPollWait(pollTimeout))
            return;
    } 

    SMoCommand::SendResponse();
}

void
SMoHVPP::ProgramFlash()
{
    int16_t         numBytes    =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    const uint8_t   mode        =   SMoCommand::gBody[3];
    const uint8_t   pollTimeout =   SMoCommand::gBody[4];
    const uint8_t * data        =  &SMoCommand::gBody[5];

    //
    // Enter Flash Programming Mode
    //
    HVPPLoadCommand(0x10);

    bool timeout = false;
    if (mode & 1) { // Paged mode
        uint32_t address = SMoGeneral::gAddress;
        for (; numBytes > 0; numBytes -= 2) {
            //
            // Load Flash Page Buffer
            //
            HVPPLoadAddress(kLowByte, SMoGeneral::gAddress & 0xFF);
            HVPPLoadData(kLowByte, *data++);
            HVPPLoadData(kHighByte, *data++);
            //
            // Latch flash data
            //
            HVPPSetControls(kPageLoad, kHighByte);
            HVPPSetControls(kDone, kHighByte);

            ++SMoGeneral::gAddress;
        }
        if (mode & 0x80) { // Write page to flash
            //
            // Load Flash High Address and Program Page
            //
            HVPPLoadAddress(kHighByte, (address >> 8) & 0xFF);
            if (address & 0x80000000)
                HVPPLoadAddress(kExtByte, (address >> 16) & 0xFF);
            HVPPCommitData();

            timeout = !HVPPPollWait(pollTimeout);
        }
    } else { 
        //
        // Are there any non-paged HVPP enabled MCUs left?
        //
    }
    //
    // Leave Flash Programming Mode
    //
    HVPPLoadCommand(0x00);
    if (!timeout)
        SMoCommand::SendResponse();
}

void
SMoHVPP::ReadFlash()
{
    int16_t     numBytes    =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    uint8_t *   outData     =  &SMoCommand::gBody[2];
    
    //
    // Flash Read
    //
    HVPPLoadCommand(0x02);
    uint8_t prevPage = 0xFF;
    for (; numBytes>0; numBytes-=2) {
        //
        // Read Flash Low and High Bytes
        //
        uint8_t page = SMoGeneral::gAddress >> 8;
        if (page != prevPage) {
            if (SMoGeneral::gAddress & 0x80000000)
                HVPPLoadAddress(kExtByte, (SMoGeneral::gAddress >> 16) & 0xFF);
            HVPPLoadAddress(kHighByte, (SMoGeneral::gAddress >> 8) & 0xFF);
            prevPage = page;
        }
        HVPPLoadAddress(kLowByte, SMoGeneral::gAddress & 0xFF);
        HVPPDataMode(INPUT);
        *outData++ = HVPPReadData(kLowByte);
        *outData++ = HVPPReadData(kHighByte);
        HVPPSetControls(kDone);
        HVPPDataMode(OUTPUT);
        ++SMoGeneral::gAddress;
    }
    *outData++ = STATUS_CMD_OK;
    SMoCommand::SendResponse(STATUS_CMD_OK, outData-&SMoCommand::gBody[0]);
}

void
SMoHVPP::ProgramEEPROM()
{
    int16_t         numBytes     =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    const uint8_t   mode        =   SMoCommand::gBody[3];
    const uint8_t   pollTimeout =   SMoCommand::gBody[4];
    const uint8_t * data        =  &SMoCommand::gBody[5];

    //
    // Enter EEPROM Programming Mode
    //
    HVPPLoadCommand(0x11);
    bool timeout = false;
    if (mode & 1) { // Paged mode
        HVPPLoadAddress(kHighByte, (SMoGeneral::gAddress >> 8) & 0xFF);
        for (; numBytes > 0; --numBytes) {
            //
            // Load EEPROM Page Buffer
            //
            HVPPLoadAddress(kLowByte, SMoGeneral::gAddress & 0xFF);
            HVPPLoadData(kLowByte, *data++);
            //
            // Latch EEPROM
            //
            HVPPSetControls(kPageLoad, kLowByte);
            HVPPSetControls(kDone, kLowByte);
            ++SMoGeneral::gAddress;
        }
        if (mode & 0x80) { // Write page to EEPROM
            //
            // Load Program EEPROM Page
            //
            HVPPCommitData();

            timeout = !HVPPPollWait(pollTimeout);
        }
    } else { 
        //
        // Are there any non-paged HVPP enabled MCUs left?
        //
    }
    //
    // Leave EEPROM Programming Mode
    //
    HVPPLoadCommand(0x00);
    if (!timeout)
        SMoCommand::SendResponse();    
}

void
SMoHVPP::ReadEEPROM()
{
    int16_t     numBytes    =  (SMoCommand::gBody[1] << 8) | SMoCommand::gBody[2];
    uint8_t *   outData     =  &SMoCommand::gBody[2];
    
    //
    // EEPROM Read
    //
    HVPPLoadCommand(0x03);
    uint8_t prevPage = 0xFF;
    for (; numBytes>0; --numBytes) {
        //
        // Read EEPROM Byte
        //
        uint8_t page = SMoGeneral::gAddress >> 8;
        if (page != prevPage) {
            HVPPLoadAddress(kHighByte, (SMoGeneral::gAddress >> 8) & 0xFF);
            prevPage = page;
        }
        HVPPLoadAddress(kLowByte, SMoGeneral::gAddress & 0xFF);
        HVPPDataMode(INPUT);
        *outData++ = HVPPReadData(kLowByte);
        HVPPSetControls(kDone);
        HVPPDataMode(OUTPUT);
        ++SMoGeneral::gAddress;
    }
    *outData = STATUS_CMD_OK;
    SMoCommand::SendResponse(STATUS_CMD_OK, outData-&SMoCommand::gBody[0]);
}

static void 
ProgramFuseLock(uint8_t command, uint8_t byteSel)
{
    const uint8_t   value       = SMoCommand::gBody[2];
    const uint8_t   pulseWidth  = SMoCommand::gBody[3];
    const uint8_t   pollTimeout = SMoCommand::gBody[4];

    HVPPLoadCommand(command);
    HVPPLoadData(kLowByte, value);
    HVPPCommitDataWithPulseWidth(pulseWidth, byteSel);

    if (HVPPPollWait(pollTimeout))
        SMoCommand::SendResponse();
}

static void
ReadFuseLock(uint8_t byteSel)
{
    uint8_t * dataOut   = &SMoCommand::gBody[2];

    HVPPLoadCommand(0x04);
    HVPPDataMode(INPUT);
    *dataOut = HVPPReadData(byteSel);
    HVPPSetControls(kDone);
    HVPPDataMode(OUTPUT);

    SMoCommand::SendResponse(STATUS_CMD_OK, 3);
}

void
SMoHVPP::ProgramFuse()
{
    const uint8_t   addr        = SMoCommand::gBody[1];

    ProgramFuseLock(0x40, addr);
}

void
SMoHVPP::ReadFuse()
{
    const uint8_t   addr    = SMoCommand::gBody[1];

    ReadFuseLock(addr == 1 ? kExt2Byte : addr);
}

void
SMoHVPP::ProgramLock()   
{
    ProgramFuseLock(0x20, kLowByte);
}

void
SMoHVPP::ReadLock()       
{
    ReadFuseLock(kHighByte);
}

static void
ReadSignatureCal(uint8_t addr, uint8_t byteSel)
{
    uint8_t * dataOut   = &SMoCommand::gBody[2];

    HVPPLoadCommand(0x08);
    HVPPLoadAddress(kLowByte, addr);
    HVPPDataMode(INPUT);
    *dataOut = HVPPReadData(byteSel);
    HVPPSetControls(kDone);
    HVPPDataMode(OUTPUT);

    SMoCommand::SendResponse(STATUS_CMD_OK, 3);
}

void
SMoHVPP::ReadSignature()  
{
    const uint8_t   addr    = SMoCommand::gBody[1];

    ReadSignatureCal(addr, kLowByte);
}

void
SMoHVPP::ReadOscCal()     
{
    ReadSignatureCal(0x00, kHighByte);
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
