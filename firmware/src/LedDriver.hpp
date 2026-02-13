#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "constants.hpp"
#include "Settings.hpp"

#define TAG_LED "LED"

class LedDriver
{
private:
    bool _on = true;
    u8_t _color;
    u8_t _brightness;
    bool _changed = true;

public:
    void begin()
    {
        _color = settings.ledColor();
        _brightness = settings.ledBrightness();
    }

    void loop()
    {
        if (_changed)
        {
            _changed = false;

            static CRGB color;
            if (_on)
            {
                color.setHSV(_color, 255, _brightness * 25.5);
            }
            else
            {
                color.setRGB(0, 0, 0);
            }

            for (u8_t i = 0; i < NUM_LEDS; i++)
            {
                rgbLedWriteOrdered(LEDS_PIN, LEDS_ORDER, color.r, color.g, color.b);
            }
        }
    }

    void setOn(bool on)
    {
        _on = on;
        _changed = true;
    }

    const u8_t brightness() const
    {
        return _brightness;
    }

    const u8_t color() const
    {
        return _color;
    }

    void incBrightness()
    {
        if (_brightness < 10)
        {
            _brightness++;
            _changed = true;
        }
    }

    void decBrightness()
    {
        if (_brightness > 1)
        {
            _brightness--;
            _changed = true;
        }
    }

    void incColor()
    {
        _color++;
        _changed = true;
    }

    void decColor()
    {
        _color--;
        _changed = true;
    }

    void saveBrightness()
    {
        settings.saveLedBrightness(_brightness);
    }

    void saveColor()
    {
        settings.saveLedColor(_color);
    }
};

LedDriver leds;
