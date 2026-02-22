#pragma once

#include <Adafruit_IS31FL3731.h>
#include "constants.hpp"
#include "matrix/Adafruit_IS31FL3731_With_Brightness.hpp"
#include "matrix/GameOfLife.hpp"
#include "matrix/Fire.hpp"
#include "matrix/Date.hpp"
#include "matrix/Rain.hpp"
#include "matrix/Sandfall.hpp"
#include "matrix/Temp.hpp"
#include "matrix/Weather.hpp"

typedef enum
{
    DATE = 0,
    TEMPERATURE,
    SANDFALL,
    WEATHER,
    GAME_OF_LIFE,
    FIRE,
    RAIN,

    MATRIX_MODE_COUNT,

    MENU
} matrix_mode_t;

class MatrixDriver
{
private:
    Adafruit_IS31FL3731_With_Brightness _matrix;

    volatile matrix_mode_t _mode = (matrix_mode_t) 0;
    volatile bool _mode_changed = false;

    bool _on = true;

    DateImpl _date;
    FireImpl _fire;
    GameOfLifeImpl _gameoflife;
    RainImpl _rain;
    SandfallImpl _sandfall;
    TempImpl _temp;
    WeatherImpl _weather;

public:
    void begin()
    {
        _matrix.begin();

        applyMode();
    }

    void loop()
    {
        if (!_on)
        {
            return;
        }

        if (_mode_changed)
        {
            ESP_LOGI(TAG_MATRIX, "Mode: %d", _mode);
            _mode_changed = false;
            applyMode();
        }

        switch (_mode)
        {
        case DATE:
            _date.loop(&_matrix);
            break;

        case GAME_OF_LIFE:
            _gameoflife.loop(&_matrix);
            break;

        case FIRE:
            _fire.loop(&_matrix);
            break;

        case RAIN:
            _rain.loop(&_matrix);
            break;

        case SANDFALL:
            _sandfall.loop(&_matrix);
            break;
        }
    }

    const matrix_mode_t mode() const
    {
        return _mode;
    }

    const u8_t brightness() const
    {
        return _matrix.brightness();
    }

    void incBrightness()
    {
        _matrix.incBrightness();
    }

    void decBrightness()
    {
        _matrix.decBrightness();
    }

    void saveBrightness()
    {
        _matrix.saveBrightness();
    }

    void setOn(bool on)
    {
        _on = on;

        if (on)
        {
            applyMode();
        }
        else
        {
            if (_mode >= MATRIX_MODE_COUNT)
            {
                setMode((matrix_mode_t) 0);
            }
            _matrix.clear();
        }
    }

    void setTemp(float temp)
    {
        _temp.temperature = temp;
        if (_mode == TEMPERATURE)
        {
            _temp.init(&_matrix);
        }
    }

    void updateTime(const tm &timeinfo)
    {
        if (_mode == DATE)
        {
            _date.init(timeinfo);
        }
        else if (_mode == SANDFALL)
        {
            _sandfall.init(&_matrix);
        }
    }

    void setWeather(u8_t weatherCode)
    {
        _weather.code = weatherCode;
        if (_mode == WEATHER)
        {
            _weather.init(&_matrix);
        }
    }

    void setMode(matrix_mode_t mode)
    {
        _mode = mode;
        _mode_changed = true;
    }

    void nextMode()
    {
        u8_t next = (u8_t)_mode + 1;
        if (next >= MATRIX_MODE_COUNT)
        {
            setMode((matrix_mode_t)0);
        }
        else
        {
            setMode((matrix_mode_t)next);
        }
    }

    void prevMode()
    {
        s8_t prev = (s8_t)_mode - 1;
        if (prev < 0)
        {
            setMode((matrix_mode_t)(MATRIX_MODE_COUNT - 1));
        }
        else
        {
            setMode((matrix_mode_t)prev);
        }
    }

    void printMenu(const char str[])
    {
        _matrix.clear();
        _matrix.drawRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, 16);
        _matrix.print(str, 1, 6);
    }

private:
    void applyMode()
    {
        switch (_mode)
        {
        case DATE:
            _date.init(timeKeeper.getTime());
            break;

        case TEMPERATURE:
            _temp.init(&_matrix);
            break;

        case WEATHER:
            _weather.init(&_matrix);
            break;

        case GAME_OF_LIFE:
            _gameoflife.init(&_matrix);
            break;

        case FIRE:
            _fire.init(&_matrix);
            break;

        case RAIN:
            _rain.init(&_matrix);
            break;

        case SANDFALL:
            _sandfall.init(&_matrix);
            break;
        }
    }
};

MatrixDriver matrix;
