#pragma once

#include <FastLED.h>
#include "../constants.hpp"
#include "Adafruit_IS31FL3731_With_Brightness.hpp"

struct GameOfLifeImpl
{
    u32_t _hash1 = 0;
    u32_t _hash2 = 0;

    void init(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        auto current = matrix->_buffer;
        
        for (u8_t x = 0; x < SCREEN_WIDTH; x++)
        {
            for (u8_t y = 0; y < SCREEN_HEIGHT; y++)
            {
                current[x][y] = random(2);
            }
        }
        
        _hash1 = 0;
        _hash2 = 0;
    }

    void loop(Adafruit_IS31FL3731_With_Brightness *matrix)
    {
        EVERY_N_MILLIS(100)
        {
            auto current = matrix->_buffer;
            auto next = matrix->_buffer2;
            u32_t hash = 0;

            // compute next generation
            for (u8_t x = 0; x < SCREEN_WIDTH; x++)
            {
                for (u8_t y = 0; y < SCREEN_HEIGHT; y++)
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

                    // hash: XOR position-dependent primes for alive cells
                    if (next[x][y])
                    {
                        hash ^= (u32_t)(x + 1) * 0x1f1f1f1f;
                        hash ^= (u32_t)(y + 1) * 0x3f3f3f3f;
                        hash = (hash << 1) | (hash >> 31); // Rotate
                    }
                }
            }

            // reset if stale
            if (hash == _hash1 || hash == _hash2)
            {
                init(matrix);
            }
            else
            {
                _hash2 = _hash1;
                _hash1 = hash;

                matrix->beginFrame();

                for (u8_t x = 0; x < SCREEN_WIDTH; x++)
                {
                    for (u8_t y = 0; y < SCREEN_HEIGHT; y++)
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

    u8_t countNeighbors(u8_t x, u8_t y, u8_t current[SCREEN_WIDTH][SCREEN_HEIGHT])
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

                u8_t nx = (x + i + SCREEN_WIDTH) % SCREEN_WIDTH;
                u8_t ny = (y + j + SCREEN_HEIGHT) % SCREEN_HEIGHT;

                if (current[nx][ny])
                {
                    count++;
                }
            }
        }
        return count;
    }
};
