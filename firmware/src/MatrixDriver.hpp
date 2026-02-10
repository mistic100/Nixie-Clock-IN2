#pragma once

#include <Adafruit_IS31FL3731.h>
#include "constants.hpp"
#include "data/weather_icons.h"
#include "matrix/Adafruit_IS31FL3731_With_Brightness.hpp"
#include "matrix/GameOfLife.hpp"
#include "matrix/Fire.hpp"
#include "matrix/Date.hpp"
#include "matrix/Rain.hpp"
#include "matrix/Sandfall.hpp"

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
    volatile bool _modeChanged = false;

    bool _on = true;
    float _temperature = 0;
    u8_t _weatherCode = 0;

    GameOfLife _gameOfLife;
    Fire _fire;
    Date _date;
    Rain _rain;
    Sandfall _sandfall;

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

        if (_modeChanged)
        {
            _modeChanged = false;
            ESP_LOGI(TAG_MATRIX, "New mode is %d", _mode);
            applyMode();
        }

        switch (_mode)
        {
        case DATE:
            _date.loop(&_matrix);
            break;

        case GAME_OF_LIFE:
            _gameOfLife.loop(&_matrix);
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
        _temperature = temp;
        if (_mode == TEMPERATURE)
        {
            showTemp();
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
        _weatherCode = weatherCode;
        if (_mode == WEATHER)
        {
            showWeather();
        }
    }

    void setMode(matrix_mode_t mode)
    {
        _mode = mode;
        _modeChanged = true;
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
            showTemp();
            break;

        case WEATHER:
            showWeather();
            break;

        case GAME_OF_LIFE:
            _gameOfLife.init(&_matrix);
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

    void showTemp()
    {
        _matrix.clear();

        char tens = (int)(_temperature / 10) % 10 + '0';
        _matrix.drawChar(tens, 0, 6);

        char ones = (int)_temperature % 10 + '0';
        _matrix.drawChar(ones, 5, 6);

        _matrix.drawPixel(10, 6);

        char decimal = (int)(_temperature * 10) % 10 + '0';
        _matrix.drawChar(decimal, 12, 6);
    }

    void showWeather()
    {
        _matrix.clear();

        switch (_weatherCode)
        {
        case 1:
            _matrix.drawBitmap(weather_sun, 3, 0, 9, 9);
            break;

        case 2:
            _matrix.drawBitmap(weather_moon, 3, 0, 9, 9);
            break;

        case 3:
            _matrix.drawBitmap(weather_cloud, 3, 0, 9, 9);
            break;

        case 4:
            _matrix.drawBitmap(weather_rain, 3, 0, 9, 9);
            break;

        case 5:
            _matrix.drawBitmap(weather_lightning, 3, 0, 9, 9);
            break;

        case 6:
            _matrix.drawBitmap(weather_snow, 3, 0, 9, 9);
            break;

        case 7:
            _matrix.drawBitmap(weather_wind, 3, 0, 9, 9);
            break;

        case 8:
            _matrix.drawBitmap(weather_fog, 3, 0, 9, 9);
            break;

        case 9:
            _matrix.drawBitmap(weather_exceptional, 3, 0, 9, 9);
            break;

        default:
            _matrix.drawChar('?', 6, 6);
            break;
        }
    }
};

MatrixDriver matrix;
