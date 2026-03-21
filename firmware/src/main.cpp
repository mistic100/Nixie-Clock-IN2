#include <Arduino.h>
#include <Wire.h>
#include <Button2.h>
#include "constants.hpp"
#include "RotaryEncoder.hpp"
#include "TimeKeeper.hpp"
#include "ZigbeeController.hpp"
#include "BmeDriver.hpp"
#include "Settings.hpp"
#include "Manager.hpp"
#include "MatrixDriver.hpp"
#include "NixieDriver.hpp"
#include "LedDriver.hpp"

#define TAG_MAIN "MAIN"

RotaryHandler encoder(ENCODER_A, ENCODER_B);
Button2 encoderButton;
Button2 button1;
Button2 button2;
Button2 button3;

void IRAM_ATTR encoderISR()
{
    encoder.tick();
}

void setup()
{
    Serial.begin(115200);

    settings.begin();

    encoderButton.setLongClickTime(LONG_CLICK_MS);
    button1.setLongClickTime(LONG_CLICK_MS);
    button2.setLongClickTime(LONG_CLICK_MS);
    button3.setLongClickTime(10000); // Zigbee reset

    encoderButton.setDoubleClickTime(DOUBLE_CLICK_MS);
    button1.setDoubleClickTime(DOUBLE_CLICK_MS);
    button2.setDoubleClickTime(DOUBLE_CLICK_MS);
    button3.setDoubleClickTime(DOUBLE_CLICK_MS);

    encoder.begin(encoderISR);
    encoderButton.begin(ENCODER_SW);
    button1.begin(BTN_1);
    button2.begin(BTN_2);
    button3.begin(BTN_3);

    encoder.onUp([]()
                 { 
        ESP_LOGI(TAG_MAIN, "Encoder up");
        manager.up(); });

    encoder.onDown([]()
                   { 
        ESP_LOGI(TAG_MAIN, "Encoder down");
        manager.down(); });

    encoderButton.setClickHandler([](Button2 &btn)
                                  { 
        ESP_LOGI(TAG_MAIN, "Encoder click");
        manager.ok(); });

    button1.setClickHandler([](Button2 &btn)
                            { 
        ESP_LOGI(TAG_MAIN, "Btn 1 click");
        zigbeeCtrl.sendBtnEvent(1, CLICK); });

    button1.setDoubleClickHandler([](Button2 &btn)
                                  { 
        ESP_LOGI(TAG_MAIN, "Btn 1 double click");
        zigbeeCtrl.sendBtnEvent(1, DOUBLE_CLICK); });

    button1.setLongClickDetectedHandler([](Button2 &btn)
                                        { 
        ESP_LOGI(TAG_MAIN, "Btn 1 long click");
        zigbeeCtrl.sendBtnEvent(1, LONG_CLICK); });

    button2.setClickHandler([](Button2 &btn)
                            { 
        ESP_LOGI(TAG_MAIN, "Btn 2 click");
        zigbeeCtrl.sendBtnEvent(2, CLICK); });

    button2.setDoubleClickHandler([](Button2 &btn)
                                  { 
        ESP_LOGI(TAG_MAIN, "Btn 2 double click");
        zigbeeCtrl.sendBtnEvent(2, DOUBLE_CLICK); });

    button2.setLongClickDetectedHandler([](Button2 &btn)
                                        { 
        ESP_LOGI(TAG_MAIN, "Btn 2 long click");
        zigbeeCtrl.sendBtnEvent(2, LONG_CLICK); });

    button3.setClickHandler([](Button2 &btn)
                            { 
        ESP_LOGI(TAG_MAIN, "Btn 3 click");
        zigbeeCtrl.toggleMainSwitch(); });

    button3.setLongClickDetectedHandler([](Button2 &btn)
                                        {
        ESP_LOGI(TAG_MAIN, "Zigbee reset");
        esp_zb_factory_reset(); });

    zigbeeCtrl.onMainSwitchChange([](bool state)
                                  {
        ESP_LOGI(TAG_MAIN, "Main state %d", state);
        manager.setOn(state);
        matrix.setOn(state);
        nixieDriver.setOn(state); });

    zigbeeCtrl.onTime([](time_t now)
                      { timeKeeper.setSystemTime(now); });

    zigbeeCtrl.onWeather([](u8_t weatherCode)
                         { matrix.setWeather(weatherCode); });

    timeKeeper.onMinute([](const tm &timeinfo)
                        {
        matrix.updateTime(timeinfo);
        nixieDriver.updateTime(timeinfo); });

    bme.onData([](bme_data_t data)
               {
        zigbeeCtrl.setTempAndHumidity(data.temp, data.humi);
        matrix.setTemp(data.temp); });

    Wire.begin(SDA1, SCL1);
    matrix.begin();
    bme.begin();
    zigbeeCtrl.begin();
    manager.begin();
    leds.begin();
    nixieDriver.begin();

    delay(2000);

    ESP_LOGI(TAG_MAIN, "Ready");
}

void loop()
{
    encoderButton.loop();
    button1.loop();
    button2.loop();
    button3.loop();

    timeKeeper.loop();
    bme.loop();
    manager.loop();
    matrix.loop();
    leds.loop();
    nixieDriver.loop();
}
