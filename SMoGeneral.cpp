// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoGeneral.cpp     - Protocol independent global commands
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#include "SMoGeneral.h"
#include "SMoCommand.h"

#include <string.h>

uint32_t    SMoGeneral::gAddress;
const uint8_t           kBuildNumberLow     = 0x01;
const uint8_t           kBuildNumberHigh    = 0x00;
const uint8_t           kHardwareVersion    = 0x00;
const uint8_t           kSoftwareMajor      = 0x00;
const uint8_t           kSoftwareMinor      = 0x01;
const uint8_t           kVoltage            =   50;
uint8_t     SMoGeneral::gSCKDuration        = 2;    // 125kHz
static uint8_t          gControllerInit     = 0;
static uint8_t          gPrescale           = 0;    // Settable
static uint8_t          gClockMatch         = 0;    //  ... but ignored
uint8_t     SMoGeneral::gControlStack[32];

void    
SMoGeneral::SignOn()
{
    memcpy(&SMoCommand::gBody[2], "\010STK500_2", 9);
    SMoCommand::SendResponse(STATUS_CMD_OK, 11);
}

void    
SMoGeneral::SetParam()
{
    uint8_t value = SMoCommand::gBody[2];
    switch (SMoCommand::gBody[1]) {
    case PARAM_VTARGET:
    case PARAM_VADJUST:
        //
        // Pick any voltage, as long as it's 5V
        //
        SMoCommand::SendResponse(value==kVoltage ? STATUS_CMD_OK : STATUS_CMD_FAILED);    
        break;
    case PARAM_SCK_DURATION:
        gSCKDuration    = value;
        break;
    case PARAM_RESET_POLARITY:
        SMoCommand::SendResponse(value ? STATUS_CMD_OK : STATUS_CMD_FAILED);    
        return;
    case PARAM_CONTROLLER_INIT:
        gControllerInit = value;
        break;
    case PARAM_OSC_PSCALE:
        gPrescale = value;
        break;
    case PARAM_OSC_CMATCH:
        gClockMatch = value;
        break;
    default:
        SMoCommand::SendResponse(STATUS_CMD_FAILED);
        return;
    }
    SMoCommand::SendResponse();
}

void    
SMoGeneral::GetParam()
{
#define result SMoCommand::gBody[2]
    switch (SMoCommand::gBody[1]) {
    case PARAM_BUILD_NUMBER_LOW:
        result  = kBuildNumberLow;
        break;
    case PARAM_BUILD_NUMBER_HIGH:
        result  = kBuildNumberHigh;
        break;
    case PARAM_HW_VER:
        result  = kHardwareVersion;
        break;
    case PARAM_SW_MAJOR:
        result  = kSoftwareMajor;
        break;
    case PARAM_SW_MINOR:
        result  = kSoftwareMinor;
        break;
    case PARAM_VTARGET:
    case PARAM_VADJUST:
        result  = kVoltage;
        break;
    case PARAM_SCK_DURATION:
        result  = gSCKDuration;
        break;
    case PARAM_RESET_POLARITY:
        result  = 1;    // Always active low
        break;
    case PARAM_CONTROLLER_INIT:
        result  = gControllerInit;
        break;
    case PARAM_OSC_PSCALE:
        result = gPrescale;
        break;
    case PARAM_OSC_CMATCH:
        result = gClockMatch;
        break;
    case PARAM_TOPCARD_DETECT:
        result = 0;
        break;
    default:
        SMoCommand::SendResponse(STATUS_CMD_FAILED);
        return;
    }
    SMoCommand::SendResponse(STATUS_CMD_OK, 3);
#undef result
}

void    
SMoGeneral::LoadAddress()
{
    SMoGeneral::gAddress = 
               (uint32_t(SMoCommand::gBody[1]) << 24UL) 
             | (uint32_t(SMoCommand::gBody[2]) << 16UL)
             | (uint32_t(SMoCommand::gBody[3]) <<  8UL)
             |  SMoCommand::gBody[4];
    SMoCommand::SendResponse();
}

void 
SMoGeneral::SetControlStack()
{
    memcpy(&SMoGeneral::gControlStack[0], &SMoCommand::gBody[1], 32);
    SMoCommand::SendResponse();
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
