// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_ISP.h      - Programmer hardware interface for ISP protocol
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#include "SMoConfig.h"
#include "SMoGeneral.h"
#include "SMoDebug.h"

enum ISP_RESET_PIN {};
enum ISP_CLOCK_PIN {};

template <ISP_RESET_PIN ISP_RESET, ISP_CLOCK_PIN MCU_CLOCK> class SMoHWIF_ISP {
    static bool     sUsingHardwareSPI;
    static int8_t   sSoftwareSPIDelay;
public:
    enum {
        RESET = ISP_RESET,
        CLOCK = MCU_CLOCK,
    };
    static void     SetupHardwareSPI() {
        sUsingHardwareSPI = true;

        digitalWrite(MISO,      LOW);
        pinMode(MISO,           INPUT);
        SPI.begin();
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);
        SPI.setClockDivider(
            SMoGeneral::gSCKDuration == 0 ? SPI_CLOCK_DIV8  :   // 2MHz
           (SMoGeneral::gSCKDuration == 1 ? SPI_CLOCK_DIV32 :   // 500kHz  
                                            SPI_CLOCK_DIV128)); // 125kHz (Default)
    }
    static void     SetupSoftwareSPI() {
        sUsingHardwareSPI = false;
        sSoftwareSPIDelay = 1;
        
        SPI.end();
        pinMode(MOSI, OUTPUT);
        pinMode(SCK, OUTPUT);
        pinMode(MISO, INPUT);
    }
    static bool     SlowdownSoftwareSPI() {
        if (++sSoftwareSPIDelay >= 8)
            return false;
#ifdef DEBUG_ISP
        SMoDebug.print("Retrying in limp mode ");
        SMoDebug.print(sSoftwareSPIDelay);
        SMoDebug.print(" (");
        SMoDebug.print(1000.0 / (4 << sSoftwareSPIDelay));
        SMoDebug.println("kHz).");
#endif
        return true;
    }
    static void     StopSPI() {
        if (sUsingHardwareSPI) 
            SPI.end();     // Stop SPI
        pinMode(MOSI, INPUT);
        pinMode(SCK,  INPUT);
    }
    static void     SetupClock() {
        //
        // Set up clock generator on OC1A
        //
        pinMode(MCU_CLOCK, OUTPUT);
        TCCR1A = _BV(COM1A0);              // CTC mode, toggle OC1A on comparison with OCR1A
        OCR1A  = 0;                        // F(OC1A) = 16MHz / (2*8*(1+0) == 1MHz
        TIMSK1 = 0;
        TCCR1B = _BV(WGM12) | _BV(CS11);   // Prescale by 8
        TCNT1  = 0;
    }
    static void     StopClock() {
        TCCR1B = 0;    // Stop clock generator
    }
    static uint8_t  Transfer(uint8_t out) {
        if (sUsingHardwareSPI)
            return SPI.transfer(out); 
            
        const int kQuarterCycle = 1<<sSoftwareSPIDelay; 
        uint8_t in = 0;
        for (int i=0; i<8; ++i) {
            digitalWrite(MOSI, (out & 0x80) != 0);
            out <<= 1;
            delayMicroseconds(kQuarterCycle);
            digitalWrite(SCK, HIGH);
            delayMicroseconds(kQuarterCycle);
            in = (in << 1) | digitalRead(MISO);
            delayMicroseconds(kQuarterCycle);
            digitalWrite(SCK, LOW);
            delayMicroseconds(kQuarterCycle);        
        }
        return in;
    }
};

template <ISP_RESET_PIN ISP_RESET, ISP_CLOCK_PIN MCU_CLOCK> bool SMoHWIF_ISP<ISP_RESET,MCU_CLOCK>::sUsingHardwareSPI;
template <ISP_RESET_PIN ISP_RESET, ISP_CLOCK_PIN MCU_CLOCK> int8_t SMoHWIF_ISP<ISP_RESET,MCU_CLOCK>::sSoftwareSPIDelay;
