#pragma once

#include <Arduino.h>

#define TAG_TIME "TIME"

class TimeKeeper
{
private:
    time_t _next_minute = 60;

    void (*_on_minute)(void);

public:
    void setTime(tm &timeinfo)
    {
        time_t now = mktime(&timeinfo);
        struct timeval tv = {.tv_sec = now, .tv_usec = 0};
        settimeofday(&tv, NULL);

        ESP_LOGI(TAG_TIME,
            "%04d-%02d-%02d %02d:%02d:%02d",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec);

        _next_minute = (now / 60 + 1) * 60;

        if (timeinfo.tm_sec == 0)
        {
            _on_minute();
        }
    }

    void onMinute(void (*callback)(void))
    {
        _on_minute = callback;
    }

    void loop()
    {
        static time_t now;
        time(&now);

        if (_next_minute <= now)
        {
            _next_minute = (now / 60 + 1) * 60;

            _on_minute();
        }
    }

    const tm* getTime()
    {
        static time_t now;
        time(&now);
        return localtime(&now);
    }
};

TimeKeeper timeKeeper;
