#include "pch.h"
#include "RunTime/Public/Time.h"

#if defined(PLATFORM_WINDOWS)
	LARGEINTEGER _prevTime = {};
	LARGEINTEGER _currentTime = {};
	static LARGEINTEGER	_Frequency;

#elif defined(PLATFORM_LINUX)
	LARGEINTEGER _prevTime = {};
	LARGEINTEGER _currentTime = {};
#endif

double _deltaTime = 0.0;
double _accumulator = 0.0;
unsigned int _fps = 0;
unsigned int _frameCount = 0;
void Time::Init()
{
#if defined(PLATFORM_WINDOWS)
	QueryPerformanceFrequency(&_currentCount);
	QueryPerformanceCounter(&_Frequency);
#elif defined(PLATFORM_LINUX)

#endif
}

void Time::Update()
{
}
