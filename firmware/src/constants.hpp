#pragma once

#define MATRIX_ROTATION 0
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 9

#define DRIVER_DIN 2
#define DRIVER_CLK 1
#define DRIVER_OE 0

#define DOTS 3

#define ENCODER_A 9
#define ENCODER_B 14
#define ENCODER_SW 20

#define BTN_1 4
#define BTN_2 22
#define BTN_3 5

#define DOUBLE_CLICK_MS 300
#define LONG_CLICK_MS 500

#define TEMP_UPDATE_INTERVAL_S 60

#define ANTI_POISONING_INTERVAL_S 20 * 60

#define LEDS_PIN RGB_BUILTIN
#define NUM_LEDS 5
#define LEDS_ORDER LED_COLOR_ORDER_RGB

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

const char* ZIGBEE_MANUFACTURER = "StrangePlanet";
const char* ZIGBEE_MODEL = "IN-2-Clock";
