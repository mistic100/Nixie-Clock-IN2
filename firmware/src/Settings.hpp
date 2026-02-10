#pragma once

#include <ArduinoNvs.h>

#define TAG_SETTINGS "SETTINGS"

static const char *KEY_SCREEN_BRIGHTNESS = "bri";
static const char *KEY_TEMP_OFFSET = "tof";
static const char *KEY_LED_COLOR = "lco";
static const char *KEY_LED_BRIGHTNESS = "lri";

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

    const u8_t ledColor() const
    {
        return NVS.getInt(KEY_LED_COLOR, 0);
    }

    const u8_t ledBrightness() const
    {
        return NVS.getInt(KEY_LED_BRIGHTNESS, 10);
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

    void saveLedColor(const u8_t color)
    {
        ESP_LOGI(TAG_SETTINGS, "Save %s %d", KEY_LED_COLOR, color);
        NVS.setInt(KEY_LED_COLOR, color);
    }

    void saveLedBrightness(const u8_t brightness)
    {
        ESP_LOGI(TAG_SETTINGS, "Save %s %d", KEY_LED_BRIGHTNESS, brightness);
        NVS.setInt(KEY_LED_BRIGHTNESS, brightness);
    }
};

Settings settings;
