#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

// buffer2 is used to store the state of the drops
#define RAIN_HEAD_POS 0
#define RAIN_DROP_SPEED 1
#define RAIN_REMAINING_TICKS 2

struct Rain
{
    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto rain = matrix->_buffer;
        auto state = matrix->_buffer2;

        for (u8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
            {
                rain[x][y] = 0;
            }

            state[x][RAIN_HEAD_POS] = random(0, MATRIX_HEIGHT * 2);
            state[x][RAIN_DROP_SPEED] = random(2, 6);
            state[x][RAIN_REMAINING_TICKS] = state[x][RAIN_DROP_SPEED];
        }
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_MILLIS(40)
        {
            auto rain = matrix->_buffer;
            auto state = matrix->_buffer2;

            // Fade the entire screen
            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
                {
                    rain[x][y] = qsub8(rain[x][y], 20);
                }
            }

            // Move the heads down
            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                state[x][RAIN_REMAINING_TICKS]--;

                // Only move based on the column's individual speed
                if (state[x][RAIN_REMAINING_TICKS] == 0)
                {
                    // Draw the head
                    if (state[x][RAIN_HEAD_POS] < MATRIX_HEIGHT)
                    {
                        rain[x][state[x][RAIN_HEAD_POS]] = 255;
                    }

                    state[x][RAIN_HEAD_POS]++;

                    // Reset the drop when it goes off screen
                    // We use a range larger than HEIGHT to create gaps between drops
                    if (state[x][RAIN_HEAD_POS] > MATRIX_HEIGHT * 2)
                    {
                        state[x][RAIN_HEAD_POS] = 0;
                        state[x][RAIN_DROP_SPEED] = random(2, 6);
                    }

                    state[x][RAIN_REMAINING_TICKS] = state[x][RAIN_DROP_SPEED];
                }
            }

            matrix->beginFrame();

            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
                {
                    matrix->drawPixel(x, y, rain[x][y]);
                }
            }

            matrix->showFrame();
        }
    }
};
