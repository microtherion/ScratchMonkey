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
#include "SMoConfig.h"

enum HVSP_SDI_PIN {};
enum HVSP_SII_PIN {};
enum HVSP_SDO_PIN {};
enum HVSP_SCI_PIN {};

template <typename HV_Platform, 
    HVSP_SDI_PIN HVSP_SDI = HVSP_SDI_PIN(8), 
    HVSP_SII_PIN HVSP_SII = HVSP_SII_PIN(9),
    HVSP_SDO_PIN HVSP_SDO = HVSP_SDO_PIN(12), 
    HVSP_SCI_PIN HVSP_SCI = HVSP_SCI_PIN(13)> class SMoHWIF_HVSP {
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
        pinMode(HVSP_SCI, OUTPUT);
        digitalWrite(HVSP_SCI, LOW);
        pinMode(HVSP_SDI, OUTPUT);
        digitalWrite(HVSP_SDI, LOW);
        pinMode(HVSP_SII, OUTPUT);
        digitalWrite(HVSP_SII, LOW);
        pinMode(HVSP_SDO, OUTPUT);       // progEnable[2] on tinyX5
        digitalWrite(HVSP_SDO, LOW);
    
        delay(powOffDelay);
        digitalWrite(HVSP_VCC, HIGH);
        delayMicroseconds(80);
        for (uint8_t i=0; i<syncCycles; ++i) {
            digitalWrite(HVSP_SCI, HIGH);
            delayMicroseconds(10);
            digitalWrite(HVSP_SCI, LOW);
        }
        digitalWrite(HVSP_RESET, LOW);
        delayMicroseconds(1);
        pinMode(HVSP_SDO, INPUT);
    }
    static void Stop() {
        digitalWrite(HVSP_RESET, HIGH);
        digitalWrite(HVSP_VCC, LOW);
        pinMode(HVSP_SDI, INPUT);
        pinMode(HVSP_SII, INPUT);
        pinMode(HVSP_SCI, INPUT);
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
        return digitalRead(HVSP_SDO);
    }
private:
    static bool HVSPBit(bool instrInBit, bool dataInBit) {
        digitalWrite(HVSP_SII, instrInBit);
        digitalWrite(HVSP_SDI, dataInBit);
        digitalWrite(HVSP_SCI, HIGH);
        digitalWrite(HVSP_SCI, LOW);
        uint8_t dataOutBit = digitalRead(HVSP_SDO);

        return dataOutBit;
    }
};

#endif /* _SMO_HWIF_HVSP_ */
