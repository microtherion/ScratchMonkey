// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1            - STK500v2 compatible programmer for Arduino
//
// File: ScratchMonkey.ino      - Main program of sketch
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// This sketch turns an Arduino into an AVR programmer, supporting the following
// protocols:
//
//  * stk500v2    for ISP programming (largely pin compatible with ArduinoISP 
//                sketch). See SMoISP.h for pinout.
//  * stk500hvsp  for HVSP programming (high voltage serial, for 8 and 14 pin 
//                ATtinys). See SMoHVSP.h for pinout.
//  * stk500pp    for HVPP programming (high voltage parallel, for 20 pin 
//                ATtinys and all ATmegas). See SMoHVPP.h for pinout.
//
#include <SPI.h>

#include "SMoCommand.h"
#include "SMoGeneral.h"
#include "SMoISP.h"

void
setup()
{
    Serial.begin(115200);
}

void
loop()
{
    switch (SMoCommand::GetNextCommand()) {
        //
        // General commands
        //
    case CMD_SIGN_ON:
        SMoGeneral::SignOn();
        break;
    case CMD_SET_PARAMETER:
        SMoGeneral::SetParam();
        break;
    case CMD_GET_PARAMETER:
        SMoGeneral::GetParam();
        break;
    case CMD_LOAD_ADDRESS:
        SMoGeneral::LoadAddress();
        break;
        //
        // ISP Commands
        //
    case CMD_ENTER_PROGMODE_ISP:
        SMoISP::EnterProgmode();
        break;
    case CMD_LEAVE_PROGMODE_ISP:
        SMoISP::LeaveProgmode();
        break;
    case CMD_CHIP_ERASE_ISP:
        SMoISP::ChipErase();    
        break;
    case CMD_PROGRAM_FLASH_ISP:
        SMoISP::ProgramFlash();
        break;
    case CMD_READ_FLASH_ISP:
        SMoISP::ReadFlash();
        break;
    case CMD_PROGRAM_EEPROM_ISP:
        SMoISP::ProgramEEPROM();
        break;
    case CMD_READ_EEPROM_ISP:
        SMoISP::ReadEEPROM();
        break;
    case CMD_PROGRAM_FUSE_ISP:
        SMoISP::ProgramFuse();
        break;
    case CMD_READ_FUSE_ISP:
        SMoISP::ReadFuse();
        break;
    case CMD_PROGRAM_LOCK_ISP:
        SMoISP::ProgramLock();
        break;
    case CMD_READ_LOCK_ISP:
        SMoISP::ReadLock();
        break;
    case CMD_READ_SIGNATURE_ISP:
        SMoISP::ReadSignature();
        break;
    case CMD_READ_OSCCAL_ISP:
        SMoISP::ReadOscCal();
        break;
    case CMD_SPI_MULTI:
        SMoISP::SPIMulti();
        break;
    case SMoCommand::kHeaderError:
    case SMoCommand::kChecksumError:
    case SMoCommand::kIncomplete:
        break;  // Ignore
    default:
        SMoCommand::SendResponse(STATUS_CMD_UNKNOWN);
        break;
    }
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
