#include "pch.h"
#include "RunTime/Public/Time.h"

#if defined(PLATFORM_WINDOWS)
	LARGEINTEGER	Time::_Frequency = {};
#elif defined(PLATFORM_LINUX)
    #define MICRO_TO_SEC	1000000.0
    timeval Time::_tv = {};
// TODO : 리눅스 타임 갱신 로직(ctime)
#else
// TODO : Platform 독립 갱신 로직(chrono)
#endif

LARGEINTEGER	Time::_preventCount = {};
LARGEINTEGER	Time::_currentCount = {};

double			Time::_deltaTime = 0.0;
double			Time::_accumulatedTime = 0.0;
unsigned int	Time::_fps = 0;
unsigned int	Time::_frameCount = 0;

void Time::Init()
{
#if defined(PLATFORM_WINDOWS)
	QueryPerformanceFrequency(&_Frequency);
	QueryPerformanceCounter(&_currentCount);
#elif defined(PLATFORM_LINUX)
    gettimeofday(&_tv, nullptr);
    _currentCount = static_cast<LARGEINTEGER>(_tv.tv_sec * MICRO_TO_SEC + _tv.tv_usec);
    _preventCount = _currentCount;
#else
#endif
}

void Time::Update()
{
#if defined(PLATFORM_WINDOWS)
    QueryPerformanceCounter(&_currentCount);

    _deltaTime = static_cast<double>(_currentCount.QuadPart - _preventCount.QuadPart) / static_cast<double>(_Frequency.QuadPart);

#elif defined(PLATFORM_LINUX)
    gettimeofday(&_tv, nullptr);
    _currentCount = static_cast<LARGEINTEGER>(_tv.tv_sec * MICRO_TO_SEC + _tv.tv_usec);

    _deltaTime = static_cast<double>(_currentCount - _preventCount) / MICRO_TO_SEC;
#else
#endif

    _preventCount = _currentCount;

    _accumulatedTime += _deltaTime;
    ++_frameCount;

    if (_accumulatedTime >= 1.0)
    {
        _fps = _frameCount;
        _frameCount = 0;
        _accumulatedTime = 0.0;
    }
}

TSTRING Time::SystemDateTime()
{
    TCHAR result[32];
    int year, month, day, hour, minute, second, msec;

    int i = 0;


#if defined(PLATFORM_WINDOWS)
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    year = systemTime.wYear;
    month = systemTime.wMonth;
    day = systemTime.wDay;
    hour = systemTime.wHour;
    minute = systemTime.wMinute;
    second = systemTime.wSecond;
    msec = systemTime.wMilliseconds;

#elif defined(PLATFORM_LINUX)
    tm localTime = {};
    localtime_r(&_tv.tv_sec, &localTime);

    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    minute = localTime.tm_min;
    second = localTime.tm_sec;
    msec = static_cast<int>(_tv.tv_usec / 1000);
#else
#endif
    result[i++] = TTEXT('0') + (year / 1000) % 10;
    result[i++] = TTEXT('0') + (year / 100) % 10;
    result[i++] = TTEXT('0') + (year / 10) % 10;
    result[i++] = TTEXT('0') + year % 10;
    result[i++] = TTEXT('-');

    result[i++] = TTEXT('0') + (month / 10);
    result[i++] = TTEXT('0') + (month % 10);
    result[i++] = TTEXT('-');

    result[i++] = TTEXT('0') + (day / 10);
    result[i++] = TTEXT('0') + (day % 10);
    result[i++] = TTEXT(' ');

    result[i++] = TTEXT('0') + (hour / 10);
    result[i++] = TTEXT('0') + (hour % 10);
    result[i++] = TTEXT(':');

    result[i++] = TTEXT('0') + (minute / 10);
    result[i++] = TTEXT('0') + (minute % 10);
    result[i++] = TTEXT(':');

    result[i++] = TTEXT('0') + (second / 10);
    result[i++] = TTEXT('0') + (second % 10);
    result[i++] = TTEXT('.');

    result[i++] = TTEXT('0') + (msec / 100);
    result[i++] = TTEXT('0') + ((msec / 10) % 10);
    result[i++] = TTEXT('0') + (msec % 10);

    result[i] = TTEXT('\0');

    return TSTRING(result);
}

