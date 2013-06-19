// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 0.1        - STK500v2 compatible programmer for Arduino
//
// File: SMoHVPP.cpp        - High Voltage Parallel Programming
//                            (for MCUs with 20 pins and more)
//
// Copyright (c) 2013 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//

#include "SMoHVPP.h"
#include "SMoCommand.h"
#include "SMoGeneral.h"
#include "SMoConfig.h"

#ifdef DEBUG_HVPP
#include "SMoDebug.h"
#endif

#include <Arduino.h>
#include <SPI.h>

enum {
    HVPP_RESET  = SMO_HVRESET,
    HVPP_VCC    = SMO_SVCC,
#if SMO_LAYOUT==SMO_LAYOUT_MEGA
#define HVPP_CONTROL_PORT(part) part##F
#define HVPP_DATA_PORT(part)    part##K
#elif SMO_LAYOUT==SMO_LAYOUT_MONKEYHEAVEN
#define HVPP_CONTROL_PORT(part) part##B
#define HVPP_DATA_PORT(part)    part##D
#endif
#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
    HVPP_RDY    = 12,
    HVPP_RCLK   = A1,
    HVPP_XTAL   = A2,
#elif SMO_LAYOUT==SMO_LAYOUT_MONKEYHEAVEN
    HVPP_RDY    = 15,
    HVPP_XTAL   = 22
#else
    HVPP_RDY    = 12,
    HVPP_XTAL   = 13
#endif
};

//
// HVPP, for 28 pins and more, requires 8 output signals and 1 input 
// signal. 20 pin MCUs multiplex some of the output signals, and they
// don't do it all the same way. We rely on the control stack uploaded
// by avrdude to tell us what signals to set, when. 
//
// This implementation uses a 74HV595 shift register for the output 
// signal, but if you're using a board with more output pins, or are
// willing to have the TX/RX pins do double duty, you may be able to 
// dispense with that.
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

//
// Control/Data access
//
#if SMO_LAYOUT==SMO_LAYOUT_STANDARD
//
// Delegate controls to auxiliary 74HC595 shift register, but
// can transfer data pretty easily
//
enum {
    PORTD_MASK = 0xFC,
    PORTB_MASK = 0x03,
    PORTD_SHIFT = 2,
    PORTB_SHIFT = 6
};

inline void
HVPPSetControlSignals(uint8_t signals)
{
    digitalWrite(HVPP_RCLK, LOW);
    SPI.transfer(signals);
    digitalWrite(HVPP_RCLK, HIGH);
}

inline void
HVPPInitControlSignals()
{
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);// Pedal to the metal
    digitalWrite(HVPP_RCLK, LOW);
    pinMode(HVPP_RCLK, OUTPUT);
}

inline void
HVPPEndControls()
{
    SPI.end();
}

inline void
HVPPSetDataMode(uint8_t mode)
{
   if (mode == OUTPUT) {
        DDRD |= PORTD_MASK;
        DDRB |= PORTB_MASK;
    } else {
        DDRD &= ~PORTD_MASK;
        DDRB &= ~PORTB_MASK;
    }
}

inline void
HVPPSetDataBits(uint8_t dataOut)
{
    PORTD = (PORTD & ~PORTD_MASK) | ((dataOut << PORTD_SHIFT) & PORTD_MASK);
    PORTB = (PORTB & ~PORTB_MASK) | ((dataOut >> PORTB_SHIFT) & PORTB_MASK);
}

inline uint8_t
HVPPGetDataBits()
{
    // No need for masking
    return ((PINB << PORTB_SHIFT) | (PIND >> PORTD_SHIFT)) & 0xFF;
}
#elif SMO_LAYOUT==SMO_LAYOUT_LEONARDO
//
// Leonardos don't have 8 contiguous pins anywhere, so we split the 
// control signals across two ports. The data signals are not as 
// critical, so we just use digitalRead (we'd have to split them
// across at least three ports).
//
enum {
    PORTF_MASK = 0xF1,
    PORTD_MASK = 0x0C,
};

inline void
HVPPSetControlSignals(uint8_t signals)
{
    PORTF   = (PORTF & ~PORTF_MASK) | (signals & PORTF_MASK);
    PORTD   = (PORTD & ~PORTD_MASK) | (signals & PORTD_MASK);
}

inline void
HVPPInitControlSignals()
{
    DDRF   |= PORTF_MASK;
    DDRD   |= PORTD_MASK;
}

inline void
HVPPEndControls()
{
}

inline void
HVPPSetDataMode(uint8_t mode)
{
    for (uint8_t pin=2; pin<10; ++pin)
        pinMode(pin, mode);
}

inline void
HVPPSetDataBits(uint8_t dataOut)
{
    for (uint8_t pin=2; pin<10; ++pin) {
        digitalWrite(pin, dataOut & 1);
        dataOut >>= 1;
    }
}

inline uint8_t
HVPPGetDataBits()
{
    uint8_t dataIn;

    for (uint8_t pin=9; pin >= 2; --pin)
        dataIn = (dataIn << 1) | digitalRead(pin);

    return dataIn;
}
#else
//
// Megas and MonkeyHeaven have lots of contiguous pins, so we just use two full ports.
//
#define REG(part,port) part##port

#define PORT_CTRL HVPP_CONTROL_PORT(PORT)
#define DDR_CTRL  HVPP_CONTROL_PORT(DDR)
#define PORT_DATA HVPP_DATA_PORT(PORT)
#define DDR_DATA  HVPP_DATA_PORT(DDR)
#define PIN_DATA  HVPP_DATA_PORT(PIN)

inline void
HVPPSetControlSignals(uint8_t signals)
{
  PORT_CTRL   = signals & 0xFD;
  // 
  // We're a tiny bit too fast with our signals
  //
  delayMicroseconds(1);
}

inline void
HVPPInitControlSignals()
{
    DDR_CTRL    = 0xFD;
}

inline void
HVPPEndControls()
{
}

inline void
HVPPSetDataMode(uint8_t mode)
{
   if (mode == OUTPUT) {
       DDR_DATA = 0xFF;
    } else {
       DDR_DATA = 0x00;
    }
}

inline void
HVPPSetDataBits(uint8_t dataOut)
{
    PORT_DATA = dataOut;
}

inline uint8_t
HVPPGetDataBits()
{
    return PIN_DATA;
}
#endif

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
    HVPPSetControlSignals(SMoGeneral::gControlStack[controlIx]);
}

inline void
HVPPSetControls(uint8_t controlIx, uint8_t byteSel)
{
    HVPPSetControls(controlIx+byteSel);
}

inline void
HVPPInitControls()
{
    HVPPInitControlSignals();
    HVPPSetControls(kInit);            // Set all control pins to zero
}

inline void
HVPPDataMode(uint8_t mode)
{
#ifdef DEBUG_HVPP
   SMoDebug.println(mode == OUTPUT ? "Data OUT" : "Data IN");
#endif
   HVPPSetDataMode(mode);
}

inline void
HVPPSetDataRaw(uint8_t dataOut)
{
#ifdef DEBUG_HVPP
   SMoDebug.print("Data<");
   SMoDebug.println(dataOut, HEX);
#endif
   HVPPSetDataBits(dataOut);
}

inline uint8_t
HVPPGetDataRaw()
{
    uint8_t dataIn = HVPPGetDataBits();
    
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
    uint32_t target = millis()+pollTimeout+5;
    while (millis() != target)
        if (digitalRead(HVPP_RDY)) 
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
    const uint8_t   powoffDelay = SMoCommand::gBody[5];
    const uint8_t   resetDelay1 = SMoCommand::gBody[6];
    const uint8_t   resetDelay2 = SMoCommand::gBody[7];
    
    pinMode(HVPP_VCC, OUTPUT);
    digitalWrite(HVPP_VCC, LOW);
    digitalWrite(HVPP_RESET, HIGH); // Set BEFORE pinMode, so we don't glitch LOW
    pinMode(HVPP_RESET, OUTPUT);
    pinMode(HVPP_RDY, INPUT);
    digitalWrite(HVPP_RDY, LOW);
    pinMode(HVPP_XTAL, OUTPUT);
    digitalWrite(HVPP_XTAL, LOW);
    HVPPDataMode(OUTPUT);
    HVPPInitControls();

    delay(powoffDelay);
    digitalWrite(HVPP_VCC, HIGH);
    delayMicroseconds(50);
    for (uint8_t i=0; i<latchCycles; ++i) {
        digitalWrite(HVPP_XTAL, HIGH);
        delayMicroseconds(10);
        digitalWrite(HVPP_XTAL, LOW);
    }
    digitalWrite(HVPP_RESET, LOW);
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

    digitalWrite(HVPP_RESET, HIGH);
    digitalWrite(HVPP_VCC, LOW);

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
    *outData = STATUS_CMD_OK;
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
