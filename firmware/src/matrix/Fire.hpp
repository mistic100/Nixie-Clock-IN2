#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct Fire
{
    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto fire = matrix->_buffer;

        for (u8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
            {
                fire[x][y] = 0;
            }
        }
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_MILLIS(50)
        {
            auto fire = matrix->_buffer;

            // Add random embers to the bottom row
            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                fire[x][MATRIX_HEIGHT - 1] = random(64, 256);
            }

            // Propagate heat upwards
            for (u8_t y = 0; y < MATRIX_HEIGHT - 1; y++)
            {
                for (u8_t x = 0; x < MATRIX_WIDTH; x++)
                {
                    // Average the pixels below and to the sides to create a spread
                    u8_t left = (x > 0) ? fire[x - 1][y + 1] : fire[MATRIX_WIDTH - 1][y + 1];
                    u8_t right = (x < MATRIX_WIDTH - 1) ? fire[x + 1][y + 1] : fire[0][y + 1];
                    u8_t below = fire[x][y + 1];

                    u8_t newHeat = (left + right + below + below) / 4;
                    u8_t cooling = random(0, 64);

                    if (newHeat > cooling)
                    {
                        fire[x][y] = newHeat - cooling;
                    }
                    else
                    {
                        fire[x][y] = 0;
                    }
                }
            }

            matrix->beginFrame();

            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
                {
                    matrix->drawPixel(x, y, fire[x][y]);
                }
            }

            matrix->showFrame();
        }
    }
};
