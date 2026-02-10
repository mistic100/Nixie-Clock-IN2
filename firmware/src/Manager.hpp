#pragma once

#include "MatrixDriver.hpp"
#include "BmeDriver.hpp"
#include "LedDriver.hpp"

typedef enum
{
    MATRIX_BRIGHTNESS,
    TEMP_OFFSET,
    LED_COLOR,
    LED_BRIGHTNESS,
    BACK,

    MENU_COUNT
} menu_t;

class Manager
{
private:
    bool _on = true;

    volatile menu_t _menu;
    volatile bool _inSetting = false;
    volatile bool _menuChanged = false;

    volatile bool _brightnessChanged = false;
    volatile bool _tempOffsetChanged = false;
    volatile bool _ledColorChanged = false;
    volatile bool _ledBrightnessChanged = false;

    char _str_buffer[8];

public:
    void begin()
    {
    }

    void loop()
    {
        if (!_on)
        {
            return;
        }

        if (_brightnessChanged)
        {
            matrix.saveBrightness();
            _brightnessChanged = false;
        }
        if (_tempOffsetChanged)
        {
            bme.saveOffset();
            _tempOffsetChanged = false;
        }
        if (_ledColorChanged)
        {
            leds.saveColor();
            _ledColorChanged = false;
        }
        if (_ledBrightnessChanged)
        {
            leds.saveBrightness();
            _ledBrightnessChanged = false;
        }

        if (_menuChanged && matrix.mode() == MENU)
        {
            _menuChanged = false;

            if (!_inSetting)
            {
                showMenu();
            }
            else
            {
                showSetting();
            }
        }
    }

    void setOn(bool on)
    {
        _on = on;
    }

    void up()
    {
        if (!_on)
        {
            return;
        }

        if (matrix.mode() < MATRIX_MODE_COUNT)
        {
            matrix.nextMode();
        }
        else if (!_inSetting)
        {
            nextMenu();
        }
        else
        {
            switch (_menu)
            {
            case MATRIX_BRIGHTNESS:
                matrix.incBrightness();
                break;

            case TEMP_OFFSET:
                bme.incOffset();
                break;

            case LED_COLOR:
                leds.incColor();
                break;

            case LED_BRIGHTNESS:
                leds.incBrightness();
                break;
            }
            
            _menuChanged = true;
        }
    }

    void down()
    {
        if (!_on)
        {
            return;
        }

        if (matrix.mode() < MATRIX_MODE_COUNT)
        {
            matrix.prevMode();
        }
        else if (!_inSetting)
        {
            prevMenu();
        }
        else
        {
            switch (_menu)
            {
            case MATRIX_BRIGHTNESS:
                matrix.decBrightness();
                break;

            case TEMP_OFFSET:
                bme.decOffset();
                break;

            case LED_COLOR:
                leds.decColor();
                break;

            case LED_BRIGHTNESS:
                leds.decBrightness();
                break;
            }

            _menuChanged = true;
        }
    }

    void ok()
    {
        if (!_on)
        {
            return;
        }

        if (matrix.mode() < MATRIX_MODE_COUNT)
        {
            // enter menu
            matrix.setMode(MENU);
            setMenu(MATRIX_BRIGHTNESS);
        }
        else if (!_inSetting)
        {
            if (_menu == BACK)
            {
                matrix.setMode((matrix_mode_t)0);
            }
            else
            {
                _inSetting = true;
                _menuChanged = true;
            }
        }
        else
        {
            switch (_menu)
            {
            case MATRIX_BRIGHTNESS:
                _brightnessChanged = true;
                break;

            case TEMP_OFFSET:
                _tempOffsetChanged = true;
                break;

            case LED_COLOR:
                _ledColorChanged = true;
                break;

            case LED_BRIGHTNESS:
                _ledBrightnessChanged = true;
                break;
            }

            _inSetting = false;
            _menuChanged = true;
        }
    }

private:
    void setMenu(menu_t menu)
    {
        _menu = menu;
        _inSetting = false;
        _menuChanged = true;
    }

    void nextMenu()
    {
        u8_t next = (u8_t)_menu + 1;
        if (next >= MENU_COUNT)
        {
            setMenu((menu_t)0);
        }
        else
        {
            setMenu((menu_t)next);
        }
    }

    void prevMenu()
    {
        s8_t prev = (s8_t)_menu - 1;
        if (prev < 0)
        {
            setMenu((menu_t)(MENU_COUNT - 1));
        }
        else
        {
            setMenu((menu_t)prev);
        }
    }

    void showMenu()
    {
        switch (_menu)
        {
        case MATRIX_BRIGHTNESS:
            matrix.printMenu("MBR");
            break;

        case TEMP_OFFSET:
            matrix.printMenu("TOF");
            break;

        case LED_COLOR:
            matrix.printMenu("LCO");
            break;

        case LED_BRIGHTNESS:
            matrix.printMenu("LBR");
            break;

        case BACK:
            matrix.printMenu("BAK");
            break;
        }
    }

    void showSetting()
    {
        switch (_menu)
        {
        case MATRIX_BRIGHTNESS:
            sprintf(_str_buffer, "%02d", matrix.brightness());
            break;

        case TEMP_OFFSET:
            sprintf(_str_buffer, "%.1f", bme.offset());
            break;

        case LED_COLOR:
            sprintf(_str_buffer, "%03d", leds.color());
            break;

        case LED_BRIGHTNESS:
            sprintf(_str_buffer, "%02d", leds.brightness());
            break;
        }

        matrix.printMenu(_str_buffer);
    }
};

Manager manager;
