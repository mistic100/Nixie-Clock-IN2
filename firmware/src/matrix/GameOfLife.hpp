#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct GameOfLife
{
    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto current = matrix->_buffer;
        
        for (u8_t x = 0; x < MATRIX_WIDTH; x++)
        {
            for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
            {
                current[x][y] = random(2);
            }
        }
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_MILLIS(100)
        {
            auto current = matrix->_buffer;
            auto next = matrix->_buffer2;
            bool isStatic = true;

            // compute next generation
            for (u8_t x = 0; x < MATRIX_WIDTH; x++)
            {
                for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
                {
                    u8_t neighbors = countNeighbors(x, y, current);

                    if (current[x][y] == 1)
                    {
                        next[x][y] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
                    }
                    else
                    {
                        next[x][y] = (neighbors == 3) ? 1 : 0;
                    }

                    if (current[x][y] != next[x][y])
                    {
                        isStatic = false;
                    }
                }
            }

            // reset if stale
            if (isStatic)
            {
                init(matrix);
            }
            else
            {
                matrix->beginFrame();

                for (u8_t x = 0; x < MATRIX_WIDTH; x++)
                {
                    for (u8_t y = 0; y < MATRIX_HEIGHT; y++)
                    {
                        if (current[x][y])
                        {
                            matrix->drawPixel(x, y);
                        }
                        current[x][y] = next[x][y];
                    }
                }

                matrix->showFrame();
            }
        }
    }

    u8_t countNeighbors(u8_t x, u8_t y, u8_t current[MATRIX_WIDTH][MATRIX_HEIGHT])
    {
        u8_t count = 0;
        for (s8_t i = -1; i <= 1; i++)
        {
            for (s8_t j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }

                u8_t nx = (x + i + MATRIX_WIDTH) % MATRIX_WIDTH;
                u8_t ny = (y + j + MATRIX_HEIGHT) % MATRIX_HEIGHT;

                if (current[nx][ny])
                {
                    count++;
                }
            }
        }
        return count;
    }
};
