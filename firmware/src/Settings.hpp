#pragma once

#include <ArduinoNvs.h>

#define TAG_SETTINGS "SETTINGS"

static const char *KEY_SCREEN_BRIGHTNESS = "bri";
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

    const u8_t screenBrightess() const
    {
        return NVS.getInt(KEY_SCREEN_BRIGHTNESS, 10);
    }

    const float tempOffset() const
    {
        return NVS.getFloat(KEY_TEMP_OFFSET, 0);
    }

    const HsvColor_t ledColor() const
    {
        return HsvColor_t(
            NVS.getInt(KEY_LED_COLOR_H, 0),
            NVS.getInt(KEY_LED_COLOR_S, 0),
            NVS.getInt(KEY_LED_COLOR_V, 255)
        );
    }

    void saveScreenBrightness(const u8_t brightness)
    {
        ESP_LOGI(TAG_SETTINGS, "Save %s %d", KEY_SCREEN_BRIGHTNESS, brightness);
        NVS.setInt(KEY_SCREEN_BRIGHTNESS, brightness);
    }

    void saveTempOffset(float offset)
    {
        ESP_LOGI(TAG_SETTINGS, "Save %s %f", KEY_TEMP_OFFSET, offset);
        NVS.setFloat(KEY_TEMP_OFFSET, offset);
    }

    void saveLedColor(const HsvColor_t color)
    {
        ESP_LOGI(TAG_SETTINGS, "Save col %d %d %d", color.h, color.s, color.v);
        NVS.setInt(KEY_LED_COLOR_H, color.h, false);
        NVS.setInt(KEY_LED_COLOR_S, color.s, false);
        NVS.setInt(KEY_LED_COLOR_V, color.v, false);
        NVS.commit();
    }
};

Settings settings;
