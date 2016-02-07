// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_Port.h     - Programmer hardware interface for virtual or real ports
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_PORT_
#define _SMO_HWIF_PORT_

//
// We can't pass I/O registers to templates, so we have to employ
// a somewhat unsavory technique
//
#define SMoPIN(x)   _SFR_IO8(x)
#define SMoDDR(x)   _SFR_IO8(x+1)
#define SMoPORT(x)  _SFR_IO8(x+2)

//
// The easiest case: Just use a hardware port
//
template <int PORT> class SMoHWIF_Port_Simple {
public: 
    static void Setup(uint8_t mode) {
        if (mode == OUTPUT) {
            SMoDDR(PORT) = 0xFF;
        } else {
            SMoDDR(PORT) = 0x00;
        }
    }
    static void Stop() {
        Setup(INPUT);
    }
    static void Set(uint8_t data) {
        SMoPORT(PORT) = data;
    }
    static uint8_t Get() {
        return SMoPIN(PORT);
    }
};

//
// Split bits across two ports, with optional bit shifts
//
template <int PORT_H, uint8_t MASK_H, uint8_t SHIFT_H,
          int PORT_L, uint8_t MASK_L, uint8_t SHIFT_L>
class SMoHWIF_Port_Dual {
public:
    static void Setup(uint8_t mode) {
        if (mode == OUTPUT) {
            SMoDDR(PORT_L) |= MASK_L;
            SMoDDR(PORT_H) |= MASK_H;
        } else {
            SMoDDR(PORT_L) &= ~MASK_L;
            SMoDDR(PORT_H) &= ~MASK_H;
        }
    }
    static void Stop() {
        Setup(INPUT);
    }
    static void Set(uint8_t data) {
        SMoPORT(PORT_L) = (SMoPORT(PORT_L) & ~MASK_L) | ((data << SHIFT_L) & MASK_L);
        SMoPORT(PORT_H) = (SMoPORT(PORT_H) & ~MASK_H) | ((data >> SHIFT_H) & MASK_H);
    }
    static uint8_t Get() {
        return (SMoPIN(PORT_H) << SHIFT_H) | (SMoPIN(PORT_L) >> SHIFT_L);
    }
};

//
// Use individual Arduino calls. Slow.
//
template <uint8_t pin> class SMoHWIF_Port_Slow {
public:
    static void Setup(uint8_t mode) {
        for (uint8_t p=pin; p<pin+8; ++p)
            pinMode(p, mode);
    }
    static void Stop() {
        Setup(INPUT);
    }
    static void Set(uint8_t data) {
         for (uint8_t p=pin; p<pin+8; ++p) {
            digitalWrite(p, data&1);
            data >>= 1;
         }
    }
    static uint8_t Get() {
        uint8_t data = 0;
        
        for (uint8_t p=pin+7; p>=pin; --p) {
            data = (data << 1) | digitalRead(p);
        }
        return data;
    }
};

//
// Use an external 74HC595 shift register as a write-only port
//
enum PORT_CS_PIN {};

template <PORT_CS_PIN CS> class SMoHWIF_Port_SPI {
public:
    static void Setup(uint8_t mode) {
        SPI.begin();
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);
        SPI.setClockDivider(SPI_CLOCK_DIV2);// Pedal to the metal
        digitalWrite(CS, LOW);
        pinMode(CS, OUTPUT);
    }
    static void Stop() {
        SPI.end();
    }
    static void Set(uint8_t data) {
         digitalWrite(CS, LOW);
         SPI.transfer(data);
         digitalWrite(CS, HIGH);
    }
};

//
// Digital input pin
//
template <int PIN> class SMoHWIF_Input_Pin_Digital {
public:
    static void Setup() {
        pinMode(PIN, INPUT);
    }
    static void Stop() {
    }
    static bool Get() {
        return digitalRead(PIN);
    }
};

//
// Analog input pin, using the analog comparator. This is useful for
// the Arduino variants with 8 analog input, where ADC6 and ADC7 cannot
// be read as digital signals. Based on Leong Yu Siang's work.
//
template <int AIN> class SMoHWIF_Input_Pin_Analog {
public:
    static void Setup() {
        ADCSRA = 0x00;                          // Turn off ADC
        ADMUX  = _BV(REFS1) | _BV(REFS0) | AIN; // 1.1V vs ADC pin
        ADCSRB = _BV(ACME);                     // Use MUX as negative input
        ACSR   = _BV(ACBG);                     // Use 1.1V as positive input
        delay(1);                               // Let 1.1V stabilize
    }
    static void Stop() {
        ACSR   = _BV(ACD);                      // Disable comparator
    }
    static bool Get() {
        return !(ACSR & _BV(ACO));              // ACO -> input voltage < 1.1V
    }
};

#endif
