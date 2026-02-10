#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
#include "constants.hpp"
#include "TimeKeeper.hpp"

#define TAG_NIXIE "NIXIE"

const uint8_t PIN_H10[3] = {
    3,
    4,
    5};

const uint8_t PIN_H[10] = {
    6,
    10,
    11,
    12,
    13,
    14,
    15,
    9,
    8,
    7};

const uint8_t PIN_M10[6] = {
    17,
    19,
    21,
    21,
    28,
    18};

const uint8_t PIN_M[10] = {
    23,
    29,
    30,
    31,
    32,
    28,
    27,
    26,
    25,
    24};

class NixieDriver
{
private:
    // byte _data[32];
    byte _data[4];
    bool _on = true;
    bool _dots = false;

public:
    void begin()
    {
        // pinMode(DRIVER_DIN, OUTPUT);
        // pinMode(DRIVER_CLK, OUTPUT);
        pinMode(DRIVER_OE, OUTPUT);
        pinMode(DOTS, OUTPUT);

        // digitalWrite(DRIVER_DIN, LOW);
        // digitalWrite(DRIVER_CLK, LOW);
        digitalWrite(DRIVER_OE, LOW);
        digitalWrite(DOTS, LOW);

        SPI.begin(DRIVER_CLK, DRIVER_DIN);

        updateTime(timeKeeper.getTime());
    }

    void loop()
    {
        EVERY_N_SECONDS(1)
        {
            if (_on)
            {
                _dots = !_dots;
                digitalWrite(DOTS, _dots);
            }
        }
    }

    void setOn(bool on)
    {
        _on = on;

        if (on)
        {
            updateTime(timeKeeper.getTime());
        }
        else
        {
            digitalWrite(DRIVER_OE, LOW);
            digitalWrite(DOTS, LOW);
        }
    }

    void updateTime(const tm &timeinfo)
    {
        if (!_on)
        {
            return;
        }

        uint8_t h10 = timeinfo.tm_hour % 10;
        uint8_t h = timeinfo.tm_hour - h10 * 10;
        uint8_t m10 = timeinfo.tm_min % 10;
        uint8_t m = timeinfo.tm_min - m10 * 10;
        uint8_t dot = timeinfo.tm_sec % 2;

        // std::fill(_data, _data + 32, 0);

        // _data[PIN_H10[h10]] = 1;
        // _data[PIN_H[h]] = 1;
        // _data[PIN_M10[m10]] = 1;
        // _data[PIN_M[m]] = 1;
        // _data[PIN_DOT] = dot;

        // digitalWrite(DRIVER_OE, LOW);

        // for (byte i = 0; i < 32; i++)
        // {
        //     digitalWrite(DRIVER_CLK, HIGH);
        //     digitalWrite(DRIVER_DIN, _data[i]);
        //     digitalWrite(DRIVER_CLK, LOW);
        // }

        // digitalWrite(DRIVER_OE, HIGH);

        memset(_data, 0, sizeof(_data));

        setHVBit(PIN_H10[h10]);
        setHVBit(PIN_H[h]);
        setHVBit(PIN_M10[m10]);
        setHVBit(PIN_M[m]);

        SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    
        // Transfer 4 bytes (32 bits)
        for (int i = 0; i < 4; i++) {
            SPI.transfer(_data[i]);
        }
        
        SPI.endTransaction();
    }

private:
    void setHVBit(uint8_t pinNumber) {
        // Find which byte (0-3) and which bit (0-7)
        uint8_t byteIdx = pinNumber / 8;
        uint8_t bitIdx  = pinNumber % 8;
        _data[byteIdx] |= (1 << bitIdx);
    }
};

NixieDriver nixieDriver;
