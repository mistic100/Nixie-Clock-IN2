#pragma once

#include <Arduino.h>

#define TAG_TIME "TIME"

class TimeKeeper
{
private:
    struct tm _timeinfo;

    time_t _next_minute = 60;
    void (*_on_minute)(const tm &);

public:
    void setSystemTime(time_t now)
    {
        struct timeval tv = {.tv_sec = now, .tv_usec = 0};
        settimeofday(&tv, NULL);

        localtime_r(&now, &_timeinfo);

        ESP_LOGI(TAG_TIME,
            "%04d-%02d-%02d %02d:%02d:%02d",
            _timeinfo.tm_year + 1900,
            _timeinfo.tm_mon + 1,
            _timeinfo.tm_mday,
            _timeinfo.tm_hour,
            _timeinfo.tm_min,
            _timeinfo.tm_sec);

        _next_minute = (now / 60 + 1) * 60;

        if (_timeinfo.tm_sec == 0)
        {
            _on_minute(_timeinfo);
        }
    }

    void onMinute(void (*callback)(const tm &))
    {
        _on_minute = callback;
    }

    void loop()
    {
        time_t now;
        time(&now);

        if (_next_minute <= now)
        {
            _next_minute = (now / 60 + 1) * 60;

            localtime_r(&now, &_timeinfo);
            _on_minute(_timeinfo);
        }
    }

    const struct tm &getTime()
    {
        time_t now;
        time(&now);

        localtime_r(&now, &_timeinfo);

        return _timeinfo;
    }
};

TimeKeeper timeKeeper;
