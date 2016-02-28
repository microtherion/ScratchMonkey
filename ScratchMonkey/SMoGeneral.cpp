// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoGeneral.cpp     - Protocol independent global commands
//
// Copyright (c) 2013-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#include "SMoGeneral.h"
#include "SMoCommand.h"
#include "SMoHWIF.h"

#include <string.h>

uint32_t    SMoGeneral::gAddress;
const uint8_t           kBuildNumberLow     = 0x02;
const uint8_t           kBuildNumberHigh    = 0x00;
const uint8_t           kHardwareVersion    = 0x00;
const uint8_t           kSoftwareMajor      = 0x02;
const uint8_t           kSoftwareMinor      = 0x00;
const uint8_t           kVoltage            =   50;
uint16_t     SMoGeneral::gSCKDuration       = 2;    // 125kHz
static uint8_t          gControllerInit     = 0;
static uint8_t          gPrescale           = 0;    // Settable
static uint8_t          gClockMatch         = 0;    //  ... but ignored
uint8_t     SMoGeneral::gControlStack[32];
uint8_t     SMoGeneral::gXPROGMode;

void    
SMoGeneral::SignOn()
{
#if 0
    memcpy(&SMoCommand::gBody[2], "\010STK500_2", 9);
    SMoCommand::SendResponse(STATUS_CMD_OK, 11);
#else
    memcpy(&SMoCommand::gBody[2], "\015SCRATCHMONKEY", 15);
    SMoCommand::SendResponse(STATUS_CMD_OK, 16);
#endif
}

void    
SMoGeneral::SetParam()
{
    bool twoByteParam = SMoCommand::gBody[1] >= 0xC0;
    uint16_t value = twoByteParam
        ? ((SMoCommand::gBody[2] << 8) | SMoCommand::gBody[3]) 
        : SMoCommand::gBody[2];
    switch (SMoCommand::gBody[1]) {
    case PARAM_VTARGET:
    case PARAM_VADJUST:
        //
        // Pick any voltage, as long as it's 5V
        //
        SMoCommand::SendResponse(value==kVoltage ? STATUS_CMD_OK : STATUS_CMD_FAILED);    
        break;
    case PARAM_SCK_DURATION:
    case PARAM2_SCK_DURATION:
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
    case PARAM_DISCHARGEDELAY:
        // Pretend we handled this, for TPI
        break;
    case PARAM_SCRATCHMONKEY_STATUS_LEDS:
        SMoHWIF::Status::Set(value);
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

void 
SMoGeneral::SetXPROGMode()
{
    SMoGeneral::gXPROGMode = SMoCommand::gBody[1];
    SMoCommand::SendResponse();
}
