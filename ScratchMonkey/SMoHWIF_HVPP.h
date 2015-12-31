// -*- mode: c++; tab-width: 4; indent-tabs-mode: nil -*-
//
// ScratchMonkey 2.0        - STK500v2 compatible programmer for Arduino
//
// File: SMoHWIF_HVPP.h      - Programmer hardware interface for high voltage parallel protocol
//
// Copyright (c) 2013-2015 Matthias Neeracher <microtherion@gmail.com>
// All rights reserved.
//
// See license at bottom of this file or at
// http://opensource.org/licenses/bsd-license.php
//
// Hardware interface refactoring based on Leong Yu Siang's work.
//

#ifndef _SMO_HWIF_HVPP_
#define _SMO_HWIF_HVPP_

#include "SMoHWIF_HV.h"
#include "SMoHWIF_Port.h"
#include "SMoConfig.h"

enum HVPP_RDY_PIN {};
enum HVPP_XTAL_PIN {};

template <typename HV_Platform, typename Control_Port, typename Data_Port, typename Ready_Pin,
    HVPP_XTAL_PIN HVPP_XTAL = HVPP_XTAL_PIN(13)> class SMoHWIF_HVPP {
private:
    enum {
        HVPP_RESET = HV_Platform::RESET,
        HVPP_VCC   = HV_Platform::VCC
    };
public:
    enum {
        XTAL= HVPP_XTAL,
    };
    static void Setup(uint8_t initSignals, uint8_t powOffDelay, uint8_t latchCycles) {
        pinMode(HVPP_VCC, OUTPUT);
        digitalWrite(HVPP_VCC, LOW);
        digitalWrite(HVPP_RESET, HIGH); // Set BEFORE pinMode, so we don't glitch LOW
        pinMode(HVPP_RESET, OUTPUT);
        pinMode(HVPP_XTAL, OUTPUT);
        digitalWrite(HVPP_XTAL, LOW);
        DataMode(OUTPUT);
        InitControlSignals();
        Ready_Pin::Setup();
        SetControlSignals(initSignals);
    
        delay(powOffDelay);
        digitalWrite(HVPP_VCC, HIGH);
        delayMicroseconds(50);
        for (uint8_t i=0; i<latchCycles; ++i) {
            digitalWrite(HVPP_XTAL, HIGH);
            digitalWrite(HVPP_XTAL, LOW);
        }
        digitalWrite(HVPP_RESET, LOW);
    }
    static void Stop() {
        digitalWrite(HVPP_RESET, HIGH);
        digitalWrite(HVPP_VCC, LOW);

        Control_Port::Stop();
        Data_Port::Stop();
    }
    static void InitControlSignals() {
        Control_Port::Setup(OUTPUT);
    }
    static void SetControlSignals(uint8_t controls) {
        Control_Port::Set(controls);
    }
    static void DataMode(uint8_t mode) {
        Data_Port::Setup(mode);
    }
    static void SetData(uint8_t data) {
        Data_Port::Set(data);
    }
    static uint8_t GetData() {
        return Data_Port::Get();
    }
    static bool GetReady() {
        return Ready_Pin::Get();
    }
};

#endif
