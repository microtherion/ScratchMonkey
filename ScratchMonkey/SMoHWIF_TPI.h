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
#include "SMoConfig.h"

#define TPI_CLK_DELAY  delayMicroseconds(1)

enum TPI_RESET_PIN {};
enum TPI_DATA_PIN {};
enum TPI_CLK_PIN {};

template <typename HV_Platform, 
    TPI_RESET_PIN TPI_RESET = TPI_RESET_PIN(10), 
    TPI_DATA_PIN  TPI_DATA  = TPI_DATA_PIN(12), 
    TPI_CLK_PIN   TPI_CLK   = TPI_CLK_PIN(13)> class SMoHWIF_TPI {
private:
    enum {
        HV_RESET  = HV_Platform::RESET,
        TPI_SVCC  = HV_Platform::VCC
    };
public:
    static void Setup() {
        pinMode(TPI_SVCC, OUTPUT);
        digitalWrite(TPI_SVCC, LOW);
        pinMode(TPI_RESET, OUTPUT);
        pinMode(TPI_DATA, OUTPUT);
        digitalWrite(TPI_DATA, LOW);
        pinMode(TPI_CLK, OUTPUT);
        digitalWrite(TPI_CLK, LOW);
    
        digitalWrite(TPI_RESET, HIGH);
        digitalWrite(HV_RESET, HIGH);
        
        // Turn on supply voltage
        digitalWrite(TPI_SVCC, HIGH);
        delay(150);
    
        // Reset
        digitalWrite(TPI_RESET, LOW);
        digitalWrite(HV_RESET, LOW);
        delay(10);
    
        // Keep TPIDATA high for 16 clock cycles
        for (int i=0; i<32; ++i) 
            SendBit(HIGH);
    }
    static void Stop() {
        digitalWrite(TPI_RESET, HIGH);
        digitalWrite(HV_RESET, HIGH);
        digitalWrite(TPI_SVCC, LOW);
        pinMode(TPI_DATA, INPUT);
        pinMode(TPI_CLK, INPUT);
    }
    static void SendByte(uint8_t byte)
    {
        int parity = 0;
    
    #ifdef DEBUG_TPI
        SMoDebug.print("TPI W ");
        SMoDebug.println((uint8_t)byte, HEX);
    #endif
    
        pinMode(TPI_DATA, OUTPUT);
    
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
        pinMode(TPI_DATA, INPUT_PULLUP);
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
    static void SendBit(int bit) {
        digitalWrite(TPI_DATA, bit);
        digitalWrite(TPI_CLK, HIGH);
        TPI_CLK_DELAY;
        digitalWrite(TPI_CLK, LOW);
        TPI_CLK_DELAY;
    }
    static int ReadBit()
    {
        digitalWrite(TPI_CLK, HIGH);
        TPI_CLK_DELAY;
        int bit = digitalRead(TPI_DATA);
        digitalWrite(TPI_CLK, LOW);
        TPI_CLK_DELAY;
        
        return bit;
    }
};

#endif /* _SMO_HWIF_HVSP_ */
