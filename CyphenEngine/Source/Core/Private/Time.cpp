#include "pch.h"

#include "Core/Public/Time.h"
#include "HAL/Private/PlatformTime.h"

double Time::startTime = 0.0;
double Time::previousTime = 0.0;
double Time::currentTime = 0.0;
double Time::deltaTime = 0.0;

double Time::ElapsedTime()
{
	return currentTime - startTime;
}

double Time::DeltaTime()
{
	return deltaTime;
}

CDateTime Time::LocalDateTime()
{
	return PlatformTime::LocalDateTime();
}

CDateTime Time::UtcDateTime()
{
	return PlatformTime::UtcDateTime();
}

bool Time::Init()
{
	if (PlatformTime::Init() == false)
	{
		return false;
	}

	currentTime = PlatformTime::MonotonicSeconds();
	previousTime = currentTime;
	startTime = currentTime;
	deltaTime = 0.0;

	return true;
}

void Time::Tick()
{
	previousTime = currentTime;
	currentTime = PlatformTime::MonotonicSeconds();

	deltaTime = currentTime - previousTime;
}
