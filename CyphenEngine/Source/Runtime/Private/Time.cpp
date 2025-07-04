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
