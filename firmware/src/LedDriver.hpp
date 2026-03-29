#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "constants.hpp"
#include "Settings.hpp"

#define TAG_LED "LED"

class LedDriver
{
private:
    bool _state = true;
    HsvColor_t _color;
    bool _changed = false;
    bool _need_save = false;

public:
    void begin() {}

    void loop()
    {
        if (_changed)
        {
            _changed = false;
            applyColor();
        }

        EVERY_N_SECONDS(30)
        {
            if (_need_save)
            {
                _need_save = false;
                settings.setLedColor(_color);
            }
        }
    }

    void setState(bool state)
    {
        _state = state;
        _changed = true;
    }

    void setColor(u8_t hue, u8_t sat, u8_t val)
    {
        _color.h = hue;
        _color.s = sat;
        _color.v = val;
        _changed = true;
        _need_save = true;
    }

private:
    void applyColor()
    {
        if (_state)
        {
            static CRGB color;
            color.setHSV(_color.h, _color.s, _color.v);

            ESP_LOGI(TAG_LED, "Color RGB: %d %d %d", color.r, color.g, color.b);
            for (u8_t i = 0; i < NUM_LEDS; i++)
            {
                rgbLedWriteOrdered(LEDS_PIN, LEDS_ORDER, color.r, color.g, color.b);
            }
        }
        else
        {
            ESP_LOGI(TAG_LED, "Color: 0, 0, 0");
            for (u8_t i = 0; i < NUM_LEDS; i++)
            {
                rgbLedWriteOrdered(LEDS_PIN, LEDS_ORDER, 0, 0, 0);
            }
        }
    }
};

LedDriver leds;
