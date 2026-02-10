#pragma once

#include <Arduino.h>
#include <Zigbee.h>
#include "constants.hpp"
#include "CustomZigbeeEP.hpp"

#define TAG_ZB "ZB"

typedef enum
{
    CLICK = 0x0,
    DOUBLE_CLICK = 0x1,
    LONG_CLICK = 0x2,
} btn_action_t;

class ZigbeeController
{
private:
    ZigbeeLight _mainSwitch = ZigbeeLight(10);
    CustomZigbeeEP _customEp = CustomZigbeeEP(11);
    ZigbeeTempSensor _tempSensor = ZigbeeTempSensor(12);

public:
    void begin()
    {
        _mainSwitch.setManufacturerAndModel(ZIGBEE_MANUFACTURER, ZIGBEE_MODEL);

        _tempSensor.setMinMaxValue(-40, 85);
        _tempSensor.setTolerance(1);
        _tempSensor.addHumiditySensor(0, 100, 1);

        Zigbee.addEndpoint(&_mainSwitch);
        Zigbee.addEndpoint(&_customEp);
        Zigbee.addEndpoint(&_tempSensor);

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

        _tempSensor.setReporting(1, 0, 0.1);
        _tempSensor.setHumidityReporting(1, 0, 0.1);
    }

    void toggleMainSwitch() { _mainSwitch.setLight(!_mainSwitch.getLightState()); }

    void onTime(void (*callback)(time_t)) { _customEp.onTime(callback); }

    void onWeather(void (*callback)(uint8_t)) { _customEp.onWeather(callback); }

    void onMainSwitchChange(void (*callback)(bool)) { _mainSwitch.onLightChange(callback); }

    void sendBtnEvent(u8_t button_id, btn_action_t action) { _customEp.sendEvent(button_id, action); }

    void setTempAndHumidity(float temp, float humi)
    {
        _tempSensor.setTemperature(temp);
        _tempSensor.setHumidity(humi);
    }
};

ZigbeeController zigbeeCtrl;
