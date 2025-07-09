#pragma once

class Time
{
private:
	static LARGEINTEGER		_preventCount;
	static LARGEINTEGER		_currentCount;

#if defined (PLATFORM_WINDOWS)
	static  LARGEINTEGER	_Frequency;
#elif defined (PLATFORM_LINUX)
	static timeval _tv;
#endif

	static double			_deltaTime;
	static double			_accumulatedTime;
	static unsigned int		_fps;
	static unsigned int		_frameCount;

public:
	static void Init();
	static void Update();
	static double DeltaTime() { return _deltaTime; }
	static unsigned int FPS() { return _fps; }
	static double SystemDateTime();
};