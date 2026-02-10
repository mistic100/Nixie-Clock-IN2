#pragma once

#include <Arduino.h>

#define TAG_ROTENC "ROTENC"

// State lookup table (matches valid quadrature transitions)
const s8_t _KNOBDIR[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0};

class RotaryHandler
{
private:
    int _clk, _dt, _stepsPerClick;
    volatile int _counter = 0;
    volatile int _accumulator = 0;
    volatile u8_t _state = 0;

    void (*_on_up)(void);
    void (*_on_down)(void);

public:
    RotaryHandler(int clk, int dt, int stepsPerClick = 4) : _clk(clk), _dt(dt), _stepsPerClick(stepsPerClick) {}

    void begin(void (*ISR_callback)())
    {
        pinMode(_clk, INPUT_PULLUP);
        pinMode(_dt, INPUT_PULLUP);

        _state = (digitalRead(_clk) << 1) | digitalRead(_dt);

        attachInterrupt(digitalPinToInterrupt(_clk), ISR_callback, CHANGE);
        attachInterrupt(digitalPinToInterrupt(_dt), ISR_callback, CHANGE);
    }

    void IRAM_ATTR tick()
    {
        _state = (_state << 2) | (digitalRead(_clk) << 1) | digitalRead(_dt);

        s8_t movement = _KNOBDIR[_state & 0x0F];

        if (movement != 0)
        {
            _accumulator += movement;

            if (_accumulator >= _stepsPerClick)
            {
                _accumulator = 0;
                _counter = _counter + 1;
                if (_on_up)
                {
                    _on_up();
                }
            }
            else if (_accumulator <= -_stepsPerClick)
            {
                _accumulator = 0;
                _counter = _counter - 1;
                if (_on_down)
                {
                    _on_down();
                }
            }
        }
    }

    void onUp(void (*callback)(void)) { _on_up = callback; }

    void onDown(void (*callback)(void)) { _on_down = callback; }

    int getCounter() { return _counter; }
};
