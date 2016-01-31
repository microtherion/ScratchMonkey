// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_HVSP.h      - Programmer hardware interface for high voltage serial protocol
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_HVSP_
#define _SMO_HWIF_HVSP_

#include "SMoHWIF_HV.h"
#include "SMoHWIF_Port.h"
#include "SMoConfig.h"

enum HVSP_SDI_BIT {};
enum HVSP_SII_BIT {};
enum HVSP_SDO_BIT {};
enum HVSP_SCI_BIT {};

template <typename HV_Platform, int PORT, 
    HVSP_SDI_BIT HVSP_SDI, 
    HVSP_SII_BIT HVSP_SII,
    HVSP_SDO_BIT HVSP_SDO, 
    HVSP_SCI_BIT HVSP_SCI> class SMoHWIF_HVSP {
private:
    enum {
        HVSP_RESET = HV_Platform::RESET,
        HVSP_VCC   = HV_Platform::VCC
    };
public:
    enum {
        SDO = HVSP_SDO
    };
    static void Setup(uint8_t powOffDelay, uint8_t syncCycles) {
        pinMode(HVSP_VCC, OUTPUT);
        digitalWrite(HVSP_VCC, LOW);
        digitalWrite(HVSP_RESET, HIGH); // Set BEFORE pinMode, so we don't glitch LOW
        pinMode(HVSP_RESET, OUTPUT);
        //
        // Set ALL other protocol pins to OUTPUT / LOW (SDO is progEnable[2] on ATtinyX5)
        //
        const int mask = _BV(HVSP_SCI) | _BV(HVSP_SDI) | _BV(HVSP_SII) | _BV(HVSP_SDO);
        SMoDDR(PORT) |= mask;
        SMoPORT(PORT) = SMoPIN(PORT) & ~mask;

        delay(powOffDelay);
        digitalWrite(HVSP_VCC, HIGH);
        delayMicroseconds(80);
        for (uint8_t i=0; i<syncCycles; ++i) {
            SMoPIN(PORT) = _BV(HVSP_SCI);
            delayMicroseconds(10);
            SMoPIN(PORT) = _BV(HVSP_SCI);
            delayMicroseconds(10);
        }
        digitalWrite(HVSP_RESET, LOW);
        delayMicroseconds(1);
        // 
        // Now make SDO an INPUT again
        //
        SMoDDR(PORT) &= ~_BV(HVSP_SDO);
    }
    static void Stop() {
        digitalWrite(HVSP_RESET, HIGH);
        SMoDDR(PORT) &= ~(_BV(HVSP_SDI) | _BV(HVSP_SII) | _BV(HVSP_SCI));
    }
    static uint8_t Transfer(uint8_t instrIn, uint8_t dataIn) {
    #ifdef DEBUG_HVSP
        SMoDebug.print("Byte ");
        SMoDebug.print(instrIn, HEX);
        SMoDebug.print(" ");
        SMoDebug.print(dataIn, HEX);
    #endif
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
    
    #ifdef DEBUG_HVSP
        SMoDebug.print(" -> ");
        SMoDebug.println(dataOut, HEX);
    #endif
        return dataOut;
    }
    static bool GetReady() {
        return (SMoPIN(PORT) & _BV(HVSP_SDO)) != 0;
    }
private:
    static bool HVSPBit(bool instrInBit, bool dataInBit) {
        SMoPORT(PORT) = (SMoPIN(PORT) & ~(_BV(HVSP_SII)|_BV(HVSP_SDI))) 
            | (dataInBit << HVSP_SDI) | (instrInBit << HVSP_SII);
        SMoDelay50ns(); // Respect setup time for SCI
        SMoPIN(PORT) = _BV(HVSP_SCI);
        SMoDelay50ns(); // Respect setup time for SDO
        bool dataOutBit = (SMoPIN(PORT) & _BV(HVSP_SDO)) != 0;
        SMoDelay50ns(); // Respect SCI high time
        SMoPIN(PORT) = _BV(HVSP_SCI);

        return dataOutBit;
    }
};

#endif /* _SMO_HWIF_HVSP_ */
