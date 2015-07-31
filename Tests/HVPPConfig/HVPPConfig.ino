//
// Project: HVPPConfig
// File:    HVPPConfig.ino
// Created: 2015-07-30
//
// We support a wide variety of "port" mechanisms for HVPP. 
// Test that a particular one works.
//

#include <SPI.h>
#include "../../SMoConfig.h"
#include "../../SMoHWIF.h"

const uint8_t kPortMask = 0xFD; // Bit 1 is not hooked up
const uint8_t sTestPatterns[] = {
    0x01, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,  
          0x05, 0x09, 0x11, 0x21, 0x41, 0x81,
                0x0C, 0x14, 0x24, 0x44, 0x84,
                      0x18, 0x28, 0x48, 0x88,
                            0x30, 0x50, 0x90,
                                  0x60, 0xA0,
                                        0xC0,
    0x0D, 0xF0, 0xFD
};

void
setup()
{
    int     test;
    uint8_t read;

    Serial.begin(115200L);
    Serial.println("CTRL -> DATA");
    delay(5);
    Serial.end();
    SMoHWIF_HVPP_Control::Setup(OUTPUT);
    SMoHWIF_HVPP_Data::Setup(INPUT);
    for (test = 0; test<sizeof(sTestPatterns); ++test) {
        SMoHWIF_HVPP_Control::Set(sTestPatterns[test]);
        delayMicroseconds(5);
        read = SMoHWIF_HVPP_Data::Get() & kPortMask;
        if (read != sTestPatterns[test]) {
            uint8_t written = SMoHWIF_HVPP_Control::Get() & kPortMask;
            delay(5);
            Serial.begin(115200L);
            Serial.print("FAIL ");
            Serial.print(sTestPatterns[test], HEX);
            Serial.print(" [");
            Serial.print(written, HEX);
            Serial.print("] -> ");
            Serial.println(read, HEX);
            delay(5);
            Serial.end();
        }
    }
    delay(5);
    Serial.begin(115200L);
    Serial.println("DATA -> CTRL");
    delay(5);
    Serial.end();
    SMoHWIF_HVPP_Control::Setup(INPUT);
    SMoHWIF_HVPP_Data::Setup(OUTPUT);
    for (test = 0; test<sizeof(sTestPatterns); ++test) {
        SMoHWIF_HVPP_Data::Set(sTestPatterns[test]);
        delayMicroseconds(5);
        read = SMoHWIF_HVPP_Control::Get() & kPortMask;
        if (read != sTestPatterns[test]) {
            uint8_t written = SMoHWIF_HVPP_Data::Get() & kPortMask;
            delay(5);
            Serial.begin(115200L);
            delay(5);
            Serial.print("FAIL ");
            Serial.print(sTestPatterns[test], HEX);
            Serial.print(" [");
            Serial.print(written, HEX);
            Serial.print("] -> ");
            Serial.println(read, HEX);
            delay(5);
            Serial.end();
        }
    }
    delay(5);
    Serial.begin(115200L);
}

void
loop()
{
}
