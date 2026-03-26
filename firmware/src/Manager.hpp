#pragma once

#include "MatrixDriver.hpp"
#include "BmeDriver.hpp"
#include "LedDriver.hpp"

#define TAG_MANAGER "MANAGER"

typedef enum
{
    MATRIX_BRIGHTNESS,
    BACK,

    MENU_COUNT
} menu_t;

class Manager
{
private:
    bool _state = true;

    volatile menu_t _menu;
    volatile bool _in_setting = false;
    volatile bool _menu_changed = false;

    volatile bool _matrix_brightness_changed = false;

    char _str_buffer[8];

public:
    void begin()
    {
    }

    void loop()
    {
        if (!_state)
        {
            return;
        }

        if (_matrix_brightness_changed)
        {
            matrix.saveBrightness();
            _matrix_brightness_changed = false;
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

    void setState(bool state)
    {
        _state = state;
    }

    void up()
    {
        if (!_state)
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
            }
            
            _menu_changed = true;
        }
    }

    void down()
    {
        if (!_state)
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
            }

            _menu_changed = true;
        }
    }

    void ok()
    {
        if (!_state)
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
        }

        matrix.printMenu(_str_buffer);
    }
};

Manager manager;
