#pragma once

#include <Adafruit_IS31FL3731.h>
#include "constants.hpp"
#include "Settings.hpp"
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

    MATRIX_MODE_COUNT
} matrix_mode_t;

class MatrixDriver
{
private:
    Adafruit_IS31FL3731_With_Brightness _matrix;

    bool _state = true;
    bool _state_changed = false;

    volatile matrix_mode_t _mode = (matrix_mode_t) 0;
    volatile bool _mode_changed = false;

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
        if (_state_changed)
        {
            _state_changed = false;

            if (!_state)
            {
                _matrix.clear();
            }
        }

        if (!_state)
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

    void setState(bool state)
    {
        _state = state;
        _state_changed = true;

        if (state)
        {
            _mode_changed = true;
        }
        else
        {
            if (_mode >= MATRIX_MODE_COUNT)
            {
                setMode((matrix_mode_t) 0);
            }
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

    void updateTime()
    {
        if (_mode == DATE)
        {
            _date.init(timeKeeper.getTime());
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
        if (!_state)
        {
            return;
        }
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
        if (!_state)
        {
            return;
        }
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
