#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct Sandfall
{
    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto sand = matrix->_buffer;
        
        for (u8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
            {
                sand[x][y] = 0;
            }
        }
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto sand = matrix->_buffer;

        // Drop a new grain every 1/2 second
        EVERY_N_SECONDS(1)
        {
            u8_t spawnX = random(6, 10);
            if (sand[spawnX][0] == 0)
            {
                sand[spawnX][0] = 1;
            }
        }

        // Physics Update (Work from bottom to top so grains don't move twice)
        for (s8_t y = MATRIX_HEIGHT - 2; y >= 0; y--)
        {
            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                if (sand[x][y] == 1)
                {
                    // Always try straight down first
                    if (sand[x][y + 1] == 0)
                    {
                        sand[x][y + 1] = 1;
                        sand[x][y] = 0;
                    }
                    else
                    {
                        // If blocked, choose Left or Right randomly first
                        s8_t dir = (random(0, 2) == 0) ? -1 : 1;

                        if (x + dir >= 0 && x + dir < MATRIX_WIDTH && sand[x + dir][y + 1] == 0)
                        {
                            sand[x + dir][y + 1] = 1;
                            sand[x][y] = 0;
                        }
                        // Then try the opposite side if the first choice was blocked
                        else if (x - dir >= 0 && x - dir < MATRIX_WIDTH && sand[x - dir][y + 1] == 0)
                        {
                            sand[x - dir][y + 1] = 1;
                            sand[x][y] = 0;
                        }
                    }
                }
            }
        }

        matrix->beginFrame();

        for (u8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
            {
                if (sand[x][y])
                {
                    matrix->drawPixel(x, y);
                }
            }
        }

        matrix->showFrame();
    }
};
