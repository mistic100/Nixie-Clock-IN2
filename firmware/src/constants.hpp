#pragma once

// zigbee configuration
#define ZIGBEE_MANUFACTURER "StrangePlanet"
#define ZIGBEE_MODEL "IN-2-Clock"
#define ZIGBEE_ENDPOINT_MAIN 1
#define ZIGBEE_ENDPOINT_BME280 2
#define ZIGBEE_ENDPOINT_LEDS 3
#define ZIGBEE_ENDPOINT_CUSTOM 10
#define ZIGBEE_CUSTOM_CLUSTER_ID 0xFF01
#define ZIGBEE_ATTR_ACTION_ID 0x0000
#define ZIGBEE_ATTR_WEATHER_ID 0x0001
#define ZIGBEE_ATTR_TEMP_OFFSET 0x0002

// screen matrix configuration
#define SCREEN_ROTATION 0
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 9
#define SCREEN_BRIGHTNESS 20

// nixie driver configuration
#define DRIVER_DIN 2
#define DRIVER_CLK 1
#define DRIVER_OE 0

#define DOTS 3

#define ANTI_POISONING_INTERVAL_S 20 * 60
#define ANTI_POISONING_CYCLES 100

#define TEMP_DISPLAY_DURATION_S 30

// buttons configuration
#define ENCODER_A 9
#define ENCODER_B 14
#define ENCODER_SW 20

#define BTN_1 4
#define BTN_2 22
#define BTN_3 5

#define DOUBLE_CLICK_MS 300
#define LONG_CLICK_MS 500

// bme280 configuration
#define TEMP_UPDATE_INTERVAL_S 60

// leds configuration
#define LEDS_PIN RGB_BUILTIN
#define NUM_LEDS 6 // built-in + 4 + 1 for safety
#define LEDS_ORDER LED_COLOR_ORDER_RGB

// datetime configuration
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
#define LOCALIZED_DATE

#ifdef LOCALIZED_DATE
const char* DAY_NAMES[7] = {
    "DIM",
    "LUN",
    "MAR",
    "MER",
    "JEU",
    "VEN",
    "SAM"
};

const char* MONTH_NAMES[12] = {
    "JAN",
    "FEV",
    "MAR",
    "AVR",
    "MAI",
    "JUN",
    "JUI",
    "AOU",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
};
#endif

// nixie driver configuration
const uint8_t PIN_H10[6] = {
    1,
    2,
    3,
    4,
    5,
    6};

const uint8_t PIN_H[10] = {
    7,
    11,
    12,
    13,
    14,
    15,
    16,
    10,
    9,
    8};

const uint8_t PIN_M10[6] = {
    17,
    19,
    20,
    21,
    22,
    18};

const uint8_t PIN_M[10] = {
    23,
    29,
    30,
    31,
    32,
    28,
    27,
    26,
    25,
    24};
