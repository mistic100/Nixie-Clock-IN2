#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "constants.hpp"
#include "TimeKeeper.hpp"

#define TAG_NIXIE "NIXIE"

class NixieDriver
{
private:
    byte _data[32];
    bool _on = true;
    bool _dots = false;

    s8_t _antipoisoning_timer = -1;

public:
    void begin()
    {
        pinMode(DRIVER_DIN, OUTPUT);
        pinMode(DRIVER_CLK, OUTPUT);
        pinMode(DRIVER_OE, OUTPUT);
        pinMode(DOTS, OUTPUT);

        digitalWrite(DRIVER_DIN, LOW);
        digitalWrite(DRIVER_CLK, LOW);
        digitalWrite(DRIVER_OE, LOW);
        digitalWrite(DOTS, LOW);
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

        EVERY_N_SECONDS(ANTI_POISONING_INTERVAL_S)
        {
            if (_on)
            {
                ESP_LOGI(TAG_NIXIE, "Start anti poisoning");
                _antipoisoning_timer = 0;
            }
        }

        EVERY_N_MILLIS(10)
        {
            if (_antipoisoning_timer >= 0)
            {
                runAntipoisoning();
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
        u8_t h10 = timeinfo.tm_hour / 10;
        u8_t h = timeinfo.tm_hour % 10;
        u8_t m10 = timeinfo.tm_min / 10;
        u8_t m = timeinfo.tm_min % 10;

        showDigits(h10, h, m10, m);
    }

private:
    void showDigits(u8_t h10, u8_t h, u8_t m10, u8_t m) {
        ESP_LOGI(TAG_NIXIE, "Show: %d %d %d %d", h10, h, m10, m);

        std::fill(_data, _data + 32, 0);

        _data[PIN_H10[h10]] = 1;
        _data[PIN_H[h]] = 1;
        _data[PIN_M10[m10]] = 1;
        _data[PIN_M[m]] = 1;

        digitalWrite(DRIVER_OE, LOW);

        for (s8_t i = 31; i >= 1; i--)
        {
            digitalWrite(DRIVER_CLK, HIGH);
            digitalWrite(DRIVER_DIN, _data[i]);
            digitalWrite(DRIVER_CLK, LOW);
        }

        digitalWrite(DRIVER_OE, HIGH);
    }

    void runAntipoisoning()
    {
        // loop each digit 0 to 9
        showDigits(_antipoisoning_timer % 10, _antipoisoning_timer % 10, _antipoisoning_timer % 10, _antipoisoning_timer % 10);

        _antipoisoning_timer++;
        if (_antipoisoning_timer >= 100)
        {
            _antipoisoning_timer = -1;
            updateTime(timeKeeper.getTime());
        }
    }
};

NixieDriver nixieDriver;
