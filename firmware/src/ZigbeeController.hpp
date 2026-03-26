#pragma once

#include <Arduino.h>
#include <Zigbee.h>
#include "constants.hpp"
#include "CustomZigbeeEP.hpp"
#include "Settings.hpp"

#define TAG_ZB "ZB"

typedef enum
{
    CLICK = 0,
    DOUBLE_CLICK = 1,
    LONG_CLICK = 2,
} btn_action_t;

class ZigbeeController
{
private:
    ZigbeeLight _mainSwitch = ZigbeeLight(1);
    ZigbeeTempSensor _tempSensor = ZigbeeTempSensor(2);
    ZigbeeColorDimmableLight _leds = ZigbeeColorDimmableLight(3);
    CustomZigbeeEP _customEp = CustomZigbeeEP(10);

public:
    void begin()
    {
        _mainSwitch.setManufacturerAndModel(ZIGBEE_MANUFACTURER, ZIGBEE_MODEL);
        _mainSwitch.addTimeCluster();

        _tempSensor.setMinMaxValue(-40, 85);
        _tempSensor.setTolerance(0.1);
        _tempSensor.addHumiditySensor(0, 100, 0.1);

        _leds.setLightColorCapabilities(ZIGBEE_COLOR_CAPABILITY_HUE_SATURATION);

        Zigbee.addEndpoint(&_mainSwitch);
        Zigbee.addEndpoint(&_tempSensor);
        Zigbee.addEndpoint(&_leds);
        Zigbee.addEndpoint(&_customEp);

        if (!Zigbee.begin())
        {
            ESP_LOGE(TAG_ZB, "Zigbee failed to start");
            delay(1000);
            esp_restart();
        }
        else
        {
            ESP_LOGI(TAG_ZB, "Zigbee started successfully");
        }

        ESP_LOGI(TAG_ZB, "Connecting to network");
        while (!Zigbee.connected())
        {
            Serial.print(".");
            delay(100);
        }
        Serial.println();

        ESP_LOGI(TAG_ZB, "Ready");

        _mainSwitch.setLight(true);

        _tempSensor.setReporting(1, 0, 0.1);
        _tempSensor.setHumidityReporting(1, 0, 0.1);

        _leds.setLightState(true);
        _leds.setLightColor(settings.ledColor());

        _customEp.setTempOffset(settings.tempOffset() * 100);
    }

    void toggleMainSwitch() { _mainSwitch.setLight(!_mainSwitch.getLightState()); }

    void onWeather(void (*callback)(uint8_t)) { _customEp.onWeather(callback); }

    void onTempOffset(void (*callback)(int16_t)) { _customEp.onTempOffset(callback); }

    void onMainSwitchChange(void (*callback)(bool)) { _mainSwitch.onLightChange(callback); }

    void onLedColorChange(ZigbeeColorLightHsvCallback callback) { _leds.onLightChangeHsv(callback); }

    void sendBtnEvent(u8_t button_id, btn_action_t action) { _customEp.sendEvent(button_id, action); }

    void setTempAndHumidity(float temp, float humi)
    {
        _tempSensor.setTemperature(temp);
        _tempSensor.setHumidity(humi);
    }

    void requestTime()
    {
        ESP_LOGI(TAG_ZB, "Request time");
        tm timeinfo = _mainSwitch.getTime();
        timeinfo.tm_year += 30; // Zigbee returns a timestamp relative to 2000/01/01
        timeKeeper.setTime(timeinfo);
    }
};

ZigbeeController zigbeeCtrl;
