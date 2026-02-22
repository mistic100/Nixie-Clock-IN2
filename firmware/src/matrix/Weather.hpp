#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "../data/weather_icons.h"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct WeatherImpl
{
    u8_t code = 0;

    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        matrix->clear();

        switch (code)
        {
        case 1:
            matrix->drawBitmap(weather_sun, 3, 0, 9, 9);
            break;

        case 2:
            matrix->drawBitmap(weather_moon, 3, 0, 9, 9);
            break;

        case 3:
            matrix->drawBitmap(weather_cloud, 3, 0, 9, 9);
            break;

        case 4:
            matrix->drawBitmap(weather_rain, 3, 0, 9, 9);
            break;

        case 5:
            matrix->drawBitmap(weather_lightning, 3, 0, 9, 9);
            break;

        case 6:
            matrix->drawBitmap(weather_snow, 3, 0, 9, 9);
            break;

        case 7:
            matrix->drawBitmap(weather_wind, 3, 0, 9, 9);
            break;

        case 8:
            matrix->drawBitmap(weather_fog, 3, 0, 9, 9);
            break;

        case 9:
            matrix->drawBitmap(weather_exceptional, 3, 0, 9, 9);
            break;

        default:
            matrix->drawChar('?', 6, 6);
            break;
        }
    }
};
