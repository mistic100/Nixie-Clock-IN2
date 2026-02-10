#pragma once

#include <Adafruit_IS31FL3731.h>
#include <FastLED.h>
#include "../constants.hpp"
#include "../Settings.hpp"
#include "../data/smallest_pixel_75pt7b.h"

#define TAG_MATRIX "MATRIX"

class Adafruit_IS31FL3731_With_Brightness
{
private:
    Adafruit_IS31FL3731 _matrix;
    u8_t _brightness;
    u8_t _f = 0;

public:
    u8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT];
    u8_t _buffer2[MATRIX_WIDTH][MATRIX_HEIGHT];

    void begin()
    {
        while (!_matrix.begin())
        {
            ESP_LOGE(TAG_MATRIX, "Could not find IS31");
            delay(1000);
        }

        _matrix.setRotation(MATRIX_ROTATION);
        _matrix.setFont(&smallest_pixel_75pt7b);
        _matrix.setTextWrap(false);

        _brightness = settings.screenBrightess();
    }

    void beginFrame()
    {
        _matrix.setFrame(_f);
        _matrix.clear();
    }

    void showFrame()
    { 
        _matrix.displayFrame(_f);
        _f = 1 - _f;
    }

    const u8_t brightness() const
    {
        return _brightness;
    }

    void incBrightness()
    {
        if (_brightness < 10)
        {
            _brightness++;
        }
    }

    void decBrightness()
    {
        if (_brightness > 1)
        {
            _brightness--;
        }
    }

    void saveBrightness()
    {
        settings.saveScreenBrightness(_brightness);
    }

    void clear()
    {
        _matrix.clear();
    }

    void drawPixel(int16_t x, int16_t y, u8_t color = 255)
    {
        _matrix.drawPixel(x, y, getColor(color));
    }

    void drawChar(unsigned char c, int16_t x, int16_t y, u8_t color = 255)
    {
        _matrix.drawChar(x, y, c, getColor(color), 0, 1);
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, u8_t color = 255)
    {
        _matrix.drawRect(x, y, w, h, getColor(color));
    }

    void drawBitmap(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, u8_t color = 255)
    {
        _matrix.drawBitmap(x, y, bitmap, w, h, getColor(color));
    }

    void print(const char str[], int16_t x, int16_t y, u8_t color = 255)
    {
        _matrix.setTextColor(getColor(color));
        _matrix.setCursor(x, y);
        _matrix.print(str);
    }

private:
    u8_t getColor(u8_t color)
    {
        return scale8(color, _brightness * 25.5);
    }
};
