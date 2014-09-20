// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programmer for Arduino
//
// File: SMoTPI.cpp         - Tiny programming interface commands
//
// Copyright (c) 2014 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Derived from Randall Bohn's ArduinoISP sketch
//

#include "SMoTPI.h"
#include "SMoGeneral.h"
#include "SMoCommand.h"
#include "SMoConfig.h"
#ifdef DEBUG_TPI
#include "SMoDebug.h"
#endif

//
// Pin definitions
//
enum {
    TPIRESET    = 10,
    TPIDATA     = 12,
    TPICLK      = 13,
};

inline bool HVRESET_SEPARATE() { return (int)SMO_HVRESET != (int)TPIRESET; }

/* registers */
#define TPI_REG_TPIIR  0x0F

#define TPI_IDENT_CODE 0x80

#define TPI_REG_TPIPCR 0x02
#define TPI_REG_TPISR  0x00

#define TPI_REG_TPISR_NVMEN		(1 << 1)

/* TPI commands */
#define TPI_CMD_SLD		0x20
#define TPI_CMD_SLD_PI	0x24
#define TPI_CMD_SIN		0x10
#define TPI_CMD_SOUT	0x90
#define TPI_CMD_SSTCS	0xC0
#define TPI_CMD_SST		0x60
#define TPI_CMD_SST_PI	0x64

#define TPI_CMD_SLDCS	0x80
#define TPI_CMD_SSTPR	0x68
#define TPI_CMD_SKEY	0xE0

/* for TPI_CMD_SIN & TPI_CMD_SOUT */
#define TPI_SIO_ADDR(x) ((x & 0x30) << 1 | (x & 0x0F)) 

/* ATtiny4/5/9/10 I/O registers */
#define TPI_IOREG_NVMCSR		0x32
#define TPI_IOREG_NVMCMD		0x33

/* bit for NVMCSR */
#define TPI_IOREG_NVMCSR_NVMBSY	(1 << 7)

/* NVM commands */
#define TPI_NVMCMD_NO_OPERATION		0x00
#define TPI_NVMCMD_CHIP_ERASE		0x10
#define TPI_NVMCMD_SECTION_ERASE	0x14
#define TPI_NVMCMD_WORD_WRITE		0x1D

#define CLK_DELAY  delayMicroseconds(1)

static void
SendTPIBit(int bit)
{
    digitalWrite(TPIDATA, bit);
    digitalWrite(TPICLK, HIGH);
    CLK_DELAY;
    digitalWrite(TPICLK, LOW);
    CLK_DELAY;
}

static void
SendTPIByte(int byte)
{
    int parity = 0;

#ifdef DEBUG_TPI
    SMoDebug.print("TPI W ");
    SMoDebug.println((uint8_t)byte, HEX);
#endif

    pinMode(TPIDATA, OUTPUT);

    // Start bit
    SendTPIBit(LOW);

    // Data bits, LSB first
    for (int i=0; i<8; ++i) {
        int bit = byte & 1;
        byte  >>= 1;
        SendTPIBit(bit);
        parity ^= bit;
    }
    
    // Parity bit
    SendTPIBit(parity);
    
    // Stop bits
    SendTPIBit(HIGH);
    SendTPIBit(HIGH);
    pinMode(TPIDATA, INPUT_PULLUP);
}

static int
ReadTPIBit()
{
    digitalWrite(TPICLK, HIGH);
    CLK_DELAY;
    int bit = digitalRead(TPIDATA);
    digitalWrite(TPICLK, LOW);
    CLK_DELAY;
    
    return bit;
}

static int
ReadTPIByte()
{
    for (int t=0; t<256; ++t) 
        if (ReadTPIBit() == 0) {    // Start bit
#ifdef DEBUG_TPI
            SMoDebug.print("TPI R [");
            SMoDebug.print(t);
            SMoDebug.print("] ");
#endif
            int byte    = 0;
            int parity  = 0;
            for (int i=0; i<8; ++i) {
                byte >>= 1;
                int bit = ReadTPIBit();
                byte |= bit ? 0x80 : 0x00;
                parity ^= bit;
            }

            // Parity check
            parity = (parity == ReadTPIBit()); 

            // Stop bits
#ifdef DEBUG_TPI
            if (!ReadTPIBit() || !ReadTPIBit())                      
                SMoDebug.print(" ! ");
#else
            ReadTPIBit();                      
            ReadTPIBit();     
#endif

#ifdef DEBUG_TPI
            SMoDebug.print((uint8_t)byte, HEX);
            SMoDebug.println(parity?"":"!");
#endif
            return parity ? byte : -1;
       }
#ifdef DEBUG_TPI
    SMoDebug.println("TPI R ?");
#endif
    return -1;
}

void
SMoTPI::EnterProgmode()
{
#ifdef DEBUG_TPI
    SMoDebugInit();
#endif
    pinMode(SMO_SVCC, OUTPUT);
    digitalWrite(SMO_SVCC, LOW);
    pinMode(TPIRESET, OUTPUT);
    pinMode(TPIDATA, OUTPUT);
    digitalWrite(TPIDATA, LOW);
    pinMode(TPICLK, OUTPUT);
    digitalWrite(TPICLK, LOW);

    digitalWrite(TPIRESET, HIGH);
    if (HVRESET_SEPARATE())
        digitalWrite(SMO_HVRESET, HIGH);
    
    // Turn on supply voltage
    digitalWrite(SMO_SVCC, HIGH);
    delay(150);

    // Reset
    digitalWrite(TPIRESET, LOW);
    if (HVRESET_SEPARATE())
        digitalWrite(SMO_HVRESET, LOW);
    delay(10);

    // Keep TPIDATA high for 16 clock cycles
    for (int i=0; i<32; ++i) 
        SendTPIBit(HIGH);

    // Remove excessive guard bits
    SendTPIByte(TPI_CMD_SSTCS | TPI_REG_TPIPCR);
    SendTPIByte(0x07);
    
    // Check identity register
    SendTPIByte(TPI_CMD_SLDCS | TPI_REG_TPIIR);
    int response = ReadTPIByte();
    if (response != 0x80) {
#ifdef DEBUG_TPI
    SMoDebug.print("TPI expected 0x80, got ");
    SMoDebug.println(response, HEX);
#endif
        SMoCommand::SendXPROGResponse(STATUS_CMD_FAILED);
        return;
    }
    
    // SKEY + 8 byte key to enable NVM programming
    SendTPIByte(TPI_CMD_SKEY); 
    SendTPIByte(0xFF); 
    SendTPIByte(0x88);
    SendTPIByte(0xD8);
    SendTPIByte(0xCD);
    SendTPIByte(0x45);
    SendTPIByte(0xAB);
    SendTPIByte(0x89);
    SendTPIByte(0x12);

    // And poll NVMEN bit
    bool ok = false;
    for (int i=0; i<100; ++i) {
        SendTPIByte(TPI_CMD_SLDCS | TPI_REG_TPISR);
        int response = ReadTPIByte();
        if (response < 0) 
            break;
        if (response & TPI_REG_TPISR_NVMEN) {
            ok = true;
            break;
        }
    }
        
    SMoCommand::SendXPROGResponse(ok ? STATUS_CMD_OK : STATUS_CMD_FAILED);
}

void
SMoTPI::LeaveProgmode()
{
    digitalWrite(TPIRESET, HIGH);
    if (SMO_HVRESET != TPIRESET)
        digitalWrite(SMO_HVRESET, HIGH);
    digitalWrite(SMO_SVCC, LOW);

    SMoCommand::SendXPROGResponse();
}

static void
SetPointerRegister(uint8_t * addr)
{
    SendTPIByte(TPI_CMD_SSTPR+0);
    SendTPIByte(addr[3]);
    SendTPIByte(TPI_CMD_SSTPR+1);
    SendTPIByte(addr[2]);
}

static void
SetNVMCommand(uint8_t cmd)
{
    SendTPIByte(TPI_CMD_SOUT | TPI_SIO_ADDR(TPI_IOREG_NVMCMD));
    SendTPIByte(cmd);
}

static void
PollNVMStatus()
{
    int res;
    do {
        SendTPIByte(TPI_CMD_SIN | TPI_SIO_ADDR(TPI_IOREG_NVMCSR));
        res = ReadTPIByte();
    } while (res & TPI_IOREG_NVMCSR_NVMBSY);
}

void
SMoTPI::Erase()
{
    uint8_t     erase   = SMoCommand::gBody[2];

    SetPointerRegister(&SMoCommand::gBody[3]);

    SetNVMCommand(erase == XPRG_ERASE_CONFIG ? TPI_NVMCMD_SECTION_ERASE : TPI_NVMCMD_CHIP_ERASE);

    //
    // Start erase by writing a dummy byte
    //
    SendTPIByte(TPI_CMD_SST);
    SendTPIByte(0xFF);

    PollNVMStatus();

    SetNVMCommand(TPI_NVMCMD_NO_OPERATION);
    PollNVMStatus();

    SMoCommand::SendXPROGResponse();
}

void
SMoTPI::WriteMem()
{
    uint8_t     memType = SMoCommand::gBody[2];
    // uint8_t  wrMode  = SMoCommand::gBody[3];
    uint16_t    len     = (SMoCommand::gBody[8] <<  8) | SMoCommand::gBody[9];

    SetPointerRegister(&SMoCommand::gBody[4]);
    SetNVMCommand(TPI_NVMCMD_WORD_WRITE);

    uint8_t *   data    = &SMoCommand::gBody[10];
    for (uint16_t i=0; i<len; i += 2) {
        SendTPIByte(TPI_CMD_SST_PI);    // Store with Postincrement
        SendTPIByte(*data++);
        SendTPIByte(TPI_CMD_SST_PI);    // Store with Postincrement
        SendTPIByte(*data++);
        PollNVMStatus();
    }
  
    SetNVMCommand(TPI_NVMCMD_NO_OPERATION);
    PollNVMStatus();
  
    SMoCommand::SendXPROGResponse();
}

void
SMoTPI::ReadMem()
{
    uint8_t     memType = SMoCommand::gBody[2];
    uint16_t    len     = (SMoCommand::gBody[7] <<  8) | SMoCommand::gBody[8];

    SetPointerRegister(&SMoCommand::gBody[3]);

    uint8_t *   data    = &SMoCommand::gBody[3];
    for (uint16_t i=0; i<len; ++i) {
        SendTPIByte(TPI_CMD_SLD_PI);    // Load with Postincrement
        *data++ = ReadTPIByte();
    }
    
    SMoCommand::SendXPROGResponse(STATUS_CMD_OK, len+3);
}

void
SMoTPI::SetParam()
{
    //
    // It's not clear what those parameters do, and we can safely ignore them
    //
    SMoCommand::SendXPROGResponse();
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
