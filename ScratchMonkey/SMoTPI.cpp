// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2/STK600ish compatible programming sketch for Arduino
//
// File: SMoTPI.cpp         - Tiny programming interface commands
//
// Copyright (c) 2014-2016 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//

#include "SMoTPI.h"
#include "SMoGeneral.h"
#include "SMoCommand.h"
#include "SMoConfig.h"
#include "SMoHWIF.h"
#ifdef DEBUG_TPI
#include "SMoDebug.h"
#endif

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

void
SMoTPI::EnterProgmode()
{
#ifdef DEBUG_TPI
    SMoDebugInit();
#endif
    SMoHWIF::TPI::Setup();
    
    // Remove excessive guard bits
    SMoHWIF::TPI::SendByte(TPI_CMD_SSTCS | TPI_REG_TPIPCR);
    SMoHWIF::TPI::SendByte(0x07);
    
    // Check identity register
    SMoHWIF::TPI::SendByte(TPI_CMD_SLDCS | TPI_REG_TPIIR);
    int response = SMoHWIF::TPI::ReadByte();
    if (response != 0x80) {
#ifdef DEBUG_TPI
    SMoDebug.print("TPI expected 0x80, got ");
    SMoDebug.println(response, HEX);
#endif
        SMoCommand::SendXPROGResponse(STATUS_CMD_FAILED);
        return;
    }
    
    // SKEY + 8 byte key to enable NVM programming
    SMoHWIF::TPI::SendByte(TPI_CMD_SKEY); 
    SMoHWIF::TPI::SendByte(0xFF); 
    SMoHWIF::TPI::SendByte(0x88);
    SMoHWIF::TPI::SendByte(0xD8);
    SMoHWIF::TPI::SendByte(0xCD);
    SMoHWIF::TPI::SendByte(0x45);
    SMoHWIF::TPI::SendByte(0xAB);
    SMoHWIF::TPI::SendByte(0x89);
    SMoHWIF::TPI::SendByte(0x12);

    // And poll NVMEN bit
    bool ok = false;
    for (int i=0; i<100; ++i) {
        SMoHWIF::TPI::SendByte(TPI_CMD_SLDCS | TPI_REG_TPISR);
        int response = SMoHWIF::TPI::ReadByte();
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
    SMoHWIF::TPI::Stop();
    SMoCommand::SendXPROGResponse();
}

static void
SetPointerRegister(uint8_t * addr)
{
    SMoHWIF::TPI::SendByte(TPI_CMD_SSTPR+0);
    SMoHWIF::TPI::SendByte(addr[3]);
    SMoHWIF::TPI::SendByte(TPI_CMD_SSTPR+1);
    SMoHWIF::TPI::SendByte(addr[2]);
}

static void
SetNVMCommand(uint8_t cmd)
{
    SMoHWIF::TPI::SendByte(TPI_CMD_SOUT | TPI_SIO_ADDR(TPI_IOREG_NVMCMD));
    SMoHWIF::TPI::SendByte(cmd);
}

static void
PollNVMStatus()
{
    int res;
    do {
        SMoHWIF::TPI::SendByte(TPI_CMD_SIN | TPI_SIO_ADDR(TPI_IOREG_NVMCSR));
        res = SMoHWIF::TPI::ReadByte();
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
    SMoHWIF::TPI::SendByte(TPI_CMD_SST);
    SMoHWIF::TPI::SendByte(0xFF);

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
        SMoHWIF::TPI::SendByte(TPI_CMD_SST_PI);    // Store with Postincrement
        SMoHWIF::TPI::SendByte(*data++);
        SMoHWIF::TPI::SendByte(TPI_CMD_SST_PI);    // Store with Postincrement
        SMoHWIF::TPI::SendByte(*data++);
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
        SMoHWIF::TPI::SendByte(TPI_CMD_SLD_PI);    // Load with Postincrement
        *data++ = SMoHWIF::TPI::ReadByte();
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
