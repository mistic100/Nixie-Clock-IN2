#pragma once

#include <Adafruit_BME280.h>
#include <FastLED.h>
#include "Settings.hpp"

#define TAG_BME280 "BME280"

struct bme_data_t
{
    float temp;
    float humi;
};

class BmeDriver
{
private:
    Adafruit_BME280 _bme280;

    void (*_on_data)(bme_data_t);

public:
    void begin()
    {
        while (!_bme280.begin(BME280_ADDRESS_ALTERNATE))
        {
            ESP_LOGE(TAG_BME280, "Could not find BME280");
            delay(1000);
        }

        auto offset = settings.getTempOffset();
        _bme280.setTemperatureCompensation(offset);
    }

    void loop()
    {
        EVERY_N_SECONDS(TEMP_UPDATE_INTERVAL_S)
        {
            update();
        }
    }

    void onData(void (*callback)(bme_data_t))
    {
        _on_data = callback;
    }

    void setOffset(float offset)
    {
        ESP_LOGI(TAG_BME280, "Offset: %f", offset);
        
        _bme280.setTemperatureCompensation(offset);
        update();

        settings.setTempOffset(offset);
    }

private:
    void update()
    {
        bme_data_t data = {
            .temp = _bme280.readTemperature(),
            .humi = _bme280.readHumidity(),
        };

        ESP_LOGI(TAG_BME280, "Temp: %f, Humi: %f", data.temp, data.humi);

        _on_data(data);
    }
};

BmeDriver bme;
