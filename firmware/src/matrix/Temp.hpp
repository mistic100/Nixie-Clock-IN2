#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct TempImpl
{
    float temperature = 0;

    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        matrix->clear();

        char tens = (int)(temperature / 10) % 10 + '0';
        matrix->drawChar(tens, 0, 6);

        char ones = (int)temperature % 10 + '0';
        matrix->drawChar(ones, 5, 6);

        matrix->drawPixel(10, 6);

        char decimal = (int)(temperature * 10) % 10 + '0';
        matrix->drawChar(decimal, 12, 6);
    }
};
