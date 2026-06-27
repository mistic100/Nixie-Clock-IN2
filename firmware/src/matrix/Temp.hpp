#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct TempImpl
{
    float _temperature = 0;

    void init(float temperature)
    {
        _temperature = temperature;
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_SECONDS(1)
        {
            matrix->beginFrame();

            char tens = (int)(_temperature / 10) % 10 + '0';
            matrix->drawChar(tens, 0, 6);

            char ones = (int)_temperature % 10 + '0';
            matrix->drawChar(ones, 5, 6);

            matrix->drawPixel(10, 6);

            char decimal = (int)(_temperature * 10) % 10 + '0';
            matrix->drawChar(decimal, 12, 6);

            matrix->showFrame();
        }
    }
};
