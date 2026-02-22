#pragma once

#include "MatrixDriver.hpp"
#include "BmeDriver.hpp"
#include "LedDriver.hpp"

#define TAG_MANAGER "MANAGER"

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
    volatile bool _in_setting = false;
    volatile bool _menu_changed = false;

    volatile bool _matrix_brightness_changed = false;
    volatile bool _temp_offset_changed = false;
    volatile bool _led_color_changed = false;
    volatile bool _led_brightness_changed = false;

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

        if (_matrix_brightness_changed)
        {
            matrix.saveBrightness();
            _matrix_brightness_changed = false;
        }
        if (_temp_offset_changed)
        {
            bme.saveOffset();
            _temp_offset_changed = false;
        }
        if (_led_color_changed)
        {
            leds.saveColor();
            _led_color_changed = false;
        }
        if (_led_brightness_changed)
        {
            leds.saveBrightness();
            _led_brightness_changed = false;
        }

        if (_menu_changed && matrix.mode() == MENU)
        {
            ESP_LOGI(TAG_MANAGER, "Menu: %d, In setting: %d", _menu, _in_setting);
            _menu_changed = false;

            if (!_in_setting)
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
        else if (!_in_setting)
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
            
            _menu_changed = true;
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
        else if (!_in_setting)
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

            _menu_changed = true;
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
            matrix.setMode(MENU);
            setMenu(MATRIX_BRIGHTNESS);
        }
        else if (!_in_setting)
        {
            if (_menu == BACK)
            {
                matrix.setMode((matrix_mode_t)0);
            }
            else
            {
                _in_setting = true;
                _menu_changed = true;
            }
        }
        else
        {
            switch (_menu)
            {
            case MATRIX_BRIGHTNESS:
                _matrix_brightness_changed = true;
                break;

            case TEMP_OFFSET:
                _temp_offset_changed = true;
                break;

            case LED_COLOR:
                _led_color_changed = true;
                break;

            case LED_BRIGHTNESS:
                _led_brightness_changed = true;
                break;
            }

            _in_setting = false;
            _menu_changed = true;
        }
    }

private:
    void setMenu(menu_t menu)
    {
        _menu = menu;
        _in_setting = false;
        _menu_changed = true;
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
