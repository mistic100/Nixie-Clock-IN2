#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "../TimeKeeper.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct Date
{
    char _buffer[16];
    u8_t _strWidth;
    int _scrollPos = MATRIX_WIDTH;

    void init(const tm &timeinfo)
    {
        #ifdef LOCALIZED_DATE
        snprintf(_buffer, sizeof(_buffer), "%s %02d %s %d", 
            DAY_NAMES[timeinfo.tm_wday], 
            timeinfo.tm_mday, 
            MONTH_NAMES[timeinfo.tm_mon], 
            timeinfo.tm_year + 1900);
        #else
        strftime(_buffer, sizeof(_buffer), "%a %d %b %Y", &timeinfo);
        #endif
        _strWidth = strlen(_buffer) * 5;
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_MILLIS(50)
        {
            _scrollPos--;
            if (_scrollPos < -_strWidth)
            {
                _scrollPos = MATRIX_WIDTH;
            }

            matrix->beginFrame();
            matrix->print(_buffer, _scrollPos, 6);
            matrix->showFrame();
        }
    }
};
