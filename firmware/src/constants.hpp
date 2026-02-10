#pragma once

#define MATRIX_ROTATION 0
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 9

#define DRIVER_DIN 2
#define DRIVER_CLK 1
#define DRIVER_OE 0

#define DOTS 3

#define ENCODER_A 20
#define ENCODER_B 14
#define ENCODER_SW 9

#define BTN_1 4
#define BTN_2 22
#define BTN_3 5

#define TEMP_UPDATE_INTERVAL_S 60

#define NUM_LEDS 5

const char* ZIGBEE_MANUFACTURER = "StrangePlanet";
const char* ZIGBEE_MODEL = "IN-2-Clock";

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
