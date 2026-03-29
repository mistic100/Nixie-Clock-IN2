#pragma once

#include <ArduinoNvs.h>

#define TAG_SETTINGS "SETTINGS"

static const char *KEY_STATE = "on";
static const char *KEY_MODE = "mode";
static const char *KEY_TEMP_OFFSET = "tof";
static const char *KEY_LED_COLOR_H = "colh";
static const char *KEY_LED_COLOR_S = "cols";
static const char *KEY_LED_COLOR_V = "colv";

class Settings
{
public:
    void begin()
    {
        NVS.begin();
    }

    const u8_t getState() const
    {
        return NVS.getInt(KEY_STATE, 1);
    }

    const u8_t getMode() const
    {
        return NVS.getInt(KEY_MODE, 0);
    }

    const float getTempOffset() const
    {
        return NVS.getFloat(KEY_TEMP_OFFSET, 0);
    }

    const HsvColor_t getLedColor() const
    {
        return HsvColor_t(
            NVS.getInt(KEY_LED_COLOR_H, 0),
            NVS.getInt(KEY_LED_COLOR_S, 0),
            NVS.getInt(KEY_LED_COLOR_V, 255)
        );
    }

    void setState(u8_t state)
    {
        if (state != getState())
        {
            ESP_LOGI(TAG_SETTINGS, "Save %s %d", KEY_STATE, state);
            NVS.setInt(KEY_STATE, state);
        }
    }

    void setMode(u8_t mode)
    {
        if (mode != getMode())
        {
            ESP_LOGI(TAG_SETTINGS, "Save %s %d", KEY_MODE, mode);
            NVS.setInt(KEY_MODE, mode);
        }
    }

    void setTempOffset(float offset)
    {
        if (offset != getTempOffset())
        {
            ESP_LOGI(TAG_SETTINGS, "Save %s %f", KEY_TEMP_OFFSET, offset);
            NVS.setFloat(KEY_TEMP_OFFSET, offset);
        }
    }

    void setLedColor(const HsvColor_t color)
    {
        auto current = getLedColor();

        if (color.h != current.h || color.s != current.s || color.v != current.v)
        {
            ESP_LOGI(TAG_SETTINGS, "Save col %d %d %d", color.h, color.s, color.v);
            NVS.setInt(KEY_LED_COLOR_H, color.h, false);
            NVS.setInt(KEY_LED_COLOR_S, color.s, false);
            NVS.setInt(KEY_LED_COLOR_V, color.v, false);
            NVS.commit();
        }
    }
};

Settings settings;
