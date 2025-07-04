#pragma once

class Time
{
private:
	static LARGEINTEGER	_preventCount;
	static LARGEINTEGER	_currentCount;

	static double		_deltaTime;
	static double		_accumulator;

	static unsigned int		_fps;
	static unsigned int		_frameCount;

public:
	static void Init();
	static void Update();
	static double DeltaTime() { return _deltaTime; }
	static unsigned int FPS() { return _fps; }
};