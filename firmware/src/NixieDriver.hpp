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
    volatile bool _state = true;
    volatile bool _changed = false;
    bool _dots = false;
    u8_t _antipoisoning_timer = 0;
    volatile u8_t _temp_display_timer = 0;

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
            if (_state)
            {
                _dots = !_dots;
                digitalWrite(DOTS, _dots);
            }
            if (_temp_display_timer)
            {
                _temp_display_timer = _temp_display_timer - 1;
                if (!_temp_display_timer)
                {
                    ESP_LOGI(TAG_NIXIE, "Temp display end");
                    setState(false);
                }
            }
        }

        EVERY_N_SECONDS(ANTI_POISONING_INTERVAL_S)
        {
            if (_state && !_temp_display_timer)
            {
                ESP_LOGI(TAG_NIXIE, "Start anti poisoning");
                _antipoisoning_timer = ANTI_POISONING_CYCLES;
            }
        }

        EVERY_N_MILLIS(10)
        {
            if (_antipoisoning_timer)
            {
                runAntipoisoning();
            }
        }

        if (_changed && !_antipoisoning_timer)
        {
            _changed = false;
            
            if (_state)
            {
                auto timeinfo = timeKeeper.getTime();

                u8_t h10 = timeinfo->tm_hour / 10;
                u8_t h = timeinfo->tm_hour % 10;
                u8_t m10 = timeinfo->tm_min / 10;
                u8_t m = timeinfo->tm_min % 10;

                showDigits(h10, h, m10, m);
                digitalWrite(DRIVER_OE, HIGH);
            }
            else
            {
                digitalWrite(DRIVER_OE, LOW);
                digitalWrite(DOTS, LOW);
            }
        }
    }

    void setState(bool state)
    {
        _state = state;
        _changed = true;
        _temp_display_timer = 0;
    }

    void update()
    {
        if (_state)
        {
            _changed = true;
        }
    }

    void setTempDisplay()
    {
        if (!_state)
        {
            ESP_LOGI(TAG_NIXIE, "Start temp display for %ds", TEMP_DISPLAY_DURATION_S);
            _state = true;
            _changed = true;
            _temp_display_timer = TEMP_DISPLAY_DURATION_S;
        }
    }

private:
    void showDigits(u8_t h10, u8_t h, u8_t m10, u8_t m)
    {
        ESP_LOGI(TAG_NIXIE, "Show: %d%d:%d%d", h10, h, m10, m);

        std::fill(_data, _data + 32, 0);

        if (h10 < 6) _data[PIN_H10[h10] - 1] = 1;
        if (h < 10) _data[PIN_H[h] - 1] = 1;
        if (m10 < 6) _data[PIN_M10[m10] - 1] = 1;
        if (m < 10) _data[PIN_M[m] - 1] = 1;

        for (s8_t i = 31; i >= 0; i--)
        {
            digitalWrite(DRIVER_DIN, _data[i]);
            digitalWrite(DRIVER_CLK, HIGH);
            digitalWrite(DRIVER_CLK, LOW);
        }
    }

    void runAntipoisoning()
    {
        // loop each digit 0 to 9
        showDigits(_antipoisoning_timer % 10, _antipoisoning_timer % 10, _antipoisoning_timer % 10, _antipoisoning_timer % 10);

        _antipoisoning_timer--;
        if (!_antipoisoning_timer)
        {
            ESP_LOGI(TAG_NIXIE, "Anti poisoning end");
            _changed = true;
        }
    }
};

NixieDriver nixieDriver;
