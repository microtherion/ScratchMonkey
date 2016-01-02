// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_TPI.h     - Programmer hardware interface for tiny programming interface protocol
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_TPI_
#define _SMO_HWIF_TPI_

#include "SMoHWIF_HV.h"
#include "SMoHWIF_Port.h"
#include "SMoConfig.h"

enum TPI_DATA_BIT {};
enum TPI_CLK_BIT {};

template <typename HV_Platform, int PORT,
    TPI_DATA_BIT  TPI_DATA,
    TPI_CLK_BIT   TPI_CLK> class SMoHWIF_TPI {
private:
    enum {
        TPI_RESET = HV_Platform::RESET,
        TPI_SVCC  = HV_Platform::VCC
    };
public:
    static void Setup() {
        pinMode(TPI_SVCC, OUTPUT);
        digitalWrite(TPI_SVCC, LOW);
        delay(150);
        pinMode(TPI_RESET, OUTPUT);
        SMoDDR(PORT)  |= _BV(TPI_DATA) | _BV(TPI_CLK);
        SMoPORT(PORT) &= ~_BV(TPI_DATA);
        SMoPORT(PORT) &= ~_BV(TPI_CLK);

        digitalWrite(TPI_RESET, HIGH);

        // Turn on supply voltage
        digitalWrite(TPI_SVCC, HIGH);
        delay(150);
    
        // Reset
        digitalWrite(TPI_RESET, LOW);
        delay(10);
    
        // Keep TPIDATA high for 16 clock cycles
        for (int i=0; i<32; ++i) 
            SendBit(HIGH);
    }
    static void Stop() {
        digitalWrite(TPI_RESET, HIGH);
        SMoDDR(PORT) &= ~_BV(TPI_DATA);
        SMoDDR(PORT) &= ~_BV(TPI_CLK);
    }
    static void SendByte(uint8_t byte)
    {
        int parity = 0;
    
    #ifdef DEBUG_TPI
        SMoDebug.print("TPI W ");
        SMoDebug.println((uint8_t)byte, HEX);
    #endif

        pinMode(12, OUTPUT);

        // Start bit
        SendBit(LOW);
    
        // Data bits, LSB first
        for (int i=0; i<8; ++i) {
            int bit = byte & 1;
            byte  >>= 1;
            SendBit(bit);
            parity ^= bit;
        }
        
        // Parity bit
        SendBit(parity);
        
        // Stop bits
        SendBit(HIGH);
        SendBit(HIGH);
        // Switch TPI_DATA to pullup input
        SMoDDR(PORT)  &= ~_BV(TPI_DATA);
        SMoPORT(PORT) |= _BV(TPI_DATA);
    }
    static int
    ReadByte()
    {
        for (int t=0; t<256; ++t) 
            if (ReadBit() == 0) {    // Start bit
    #ifdef DEBUG_TPI
                SMoDebug.print("TPI R [");
                SMoDebug.print(t);
                SMoDebug.print("] ");
    #endif
                int byte    = 0;
                int parity  = 0;
                for (int i=0; i<8; ++i) {
                    byte >>= 1;
                    int bit = ReadBit();
                    byte |= bit ? 0x80 : 0x00;
                    parity ^= bit;
                }
    
                // Parity check
                parity = (parity == ReadBit()); 
    
                // Stop bits
    #ifdef DEBUG_TPI
                if (!ReadBit() || !ReadBit())                      
                    SMoDebug.print(" ! ");
    #else
                ReadBit();                      
                ReadBit();     
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
private:
    static void SendBit(bool bit) {
        SMoPORT(PORT) = (SMoPIN(PORT) & ~_BV(TPI_DATA)) | (bit << TPI_DATA);
        SMoDelay50ns(); // Respect setup time for TPI_CLK
        SMoPORT(PORT) |= _BV(TPI_CLK);
        SMoDelay50ns(); // Clock high pulse width >= 200ns
        SMoDelay50ns();
        SMoDelay50ns();
        SMoDelay50ns();
        SMoPORT(PORT) &= ~_BV(TPI_CLK);
    }
    static bool ReadBit()
    {
        SMoPORT(PORT) |= _BV(TPI_CLK);
        SMoDelay50ns(); // Clock high pulse width >= 200ns
        SMoDelay50ns();
        SMoDelay50ns();
        SMoDelay50ns();
        bool bit = (SMoPIN(PORT) & _BV(TPI_DATA)) != 0;
        SMoPORT(PORT) &= ~_BV(TPI_CLK);
        SMoDelay50ns();
        SMoDelay50ns();

        return bit;
    }
};

#endif /* _SMO_HWIF_HVSP_ */
