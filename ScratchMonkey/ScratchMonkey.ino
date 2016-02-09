// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0            - STK500v2/STK600ish compatible programmer for Arduino
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
#include <SoftwareSerial.h>
#include <avr/io.h>

#include "SMoCommand.h"
#include "SMoGeneral.h"
#include "SMoISP.h"
#include "SMoHVSP.h"
#include "SMoHVPP.h"
#include "SMoTPI.h"
#include "SMoConfig.h"
#include "SMoHWIF.h"

#ifdef DEBUG_COMM
#include "SMoDebug.h"
#endif

void
setup()
{
#ifdef DEBUG_COMM
    SMoDebugInit();
#endif
    SMoHWIF::Status::Setup();
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
    case CMD_SET_CONTROL_STACK:
        SMoGeneral::SetControlStack();
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
        //
        // HVSP Commands
        //
    case CMD_ENTER_PROGMODE_HVSP:
    case CMD_ENTER_PROGMODE_HVSP_STK600:
        SMoHVSP::EnterProgmode();
        break;
    case CMD_LEAVE_PROGMODE_HVSP:
    case CMD_LEAVE_PROGMODE_HVSP_STK600:
        SMoHVSP::LeaveProgmode();
        break;
    case CMD_CHIP_ERASE_HVSP:
        SMoHVSP::ChipErase();    
        break;
    case CMD_PROGRAM_FLASH_HVSP:
        SMoHVSP::ProgramFlash();
        break;
    case CMD_READ_FLASH_HVSP:
        SMoHVSP::ReadFlash();
        break;
    case CMD_PROGRAM_EEPROM_HVSP:
        SMoHVSP::ProgramEEPROM();
        break;
    case CMD_READ_EEPROM_HVSP:
        SMoHVSP::ReadEEPROM();
        break;
    case CMD_PROGRAM_FUSE_HVSP:
        SMoHVSP::ProgramFuse();
        break;
    case CMD_READ_FUSE_HVSP:
        SMoHVSP::ReadFuse();
        break;
    case CMD_PROGRAM_LOCK_HVSP:
        SMoHVSP::ProgramLock();
        break;
    case CMD_READ_LOCK_HVSP:
        SMoHVSP::ReadLock();
        break;
    case CMD_READ_SIGNATURE_HVSP:
        SMoHVSP::ReadSignature();
        break;
    case CMD_READ_OSCCAL_HVSP:
        SMoHVSP::ReadOscCal();
        break;        
        //
        // HVPP Commands
        //
    case CMD_ENTER_PROGMODE_PP:
        SMoCommand::ShareSerialPins(true);
        SMoHVPP::EnterProgmode();
        break;
    case CMD_LEAVE_PROGMODE_PP:
        SMoHVPP::LeaveProgmode();
        SMoCommand::ShareSerialPins(false);
        break;
    case CMD_CHIP_ERASE_PP:
        SMoHVPP::ChipErase();    
        break;
    case CMD_PROGRAM_FLASH_PP:
        SMoHVPP::ProgramFlash();
        break;
    case CMD_READ_FLASH_PP:
        SMoHVPP::ReadFlash();
        break;
    case CMD_PROGRAM_EEPROM_PP:
        SMoHVPP::ProgramEEPROM();
        break;
    case CMD_READ_EEPROM_PP:
        SMoHVPP::ReadEEPROM();
        break;
    case CMD_PROGRAM_FUSE_PP:
        SMoHVPP::ProgramFuse();
        break;
    case CMD_READ_FUSE_PP:
        SMoHVPP::ReadFuse();
        break;
    case CMD_PROGRAM_LOCK_PP:
        SMoHVPP::ProgramLock();
        break;
    case CMD_READ_LOCK_PP:
        SMoHVPP::ReadLock();
        break;
    case CMD_READ_SIGNATURE_PP:
        SMoHVPP::ReadSignature();
        break;
    case CMD_READ_OSCCAL_PP:
        SMoHVPP::ReadOscCal();
        break;  
        // XPROG Commands
    case CMD_XPROG_SETMODE:
        SMoGeneral::SetXPROGMode();
        break;
    case CMD_XPROG:
        switch (SMoGeneral::gXPROGMode) {
        case XPRG_MODE_TPI:
            switch (SMoCommand::gBody[1]) {
            case XPRG_CMD_ENTER_PROGMODE:
                SMoTPI::EnterProgmode();
                break;
            case XPRG_CMD_LEAVE_PROGMODE:
                SMoTPI::LeaveProgmode();
                break;
            case XPRG_CMD_ERASE:
                SMoTPI::Erase();
                break;
            case XPRG_CMD_WRITE_MEM:
                SMoTPI::WriteMem();
                break;
            case XPRG_CMD_READ_MEM:
                SMoTPI::ReadMem();
                break;
            case XPRG_CMD_SET_PARAM:
                SMoTPI::SetParam();
                break;
            default:
                goto unknownMode;
            }
            break;
        default:
            goto unknownMode;
        }
        break;
        // Pseudocommands   
    case SMoCommand::kHeaderError:
    case SMoCommand::kChecksumError:
    case SMoCommand::kIncomplete:
        break;  // Ignore
    default:
    unknownMode:
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
