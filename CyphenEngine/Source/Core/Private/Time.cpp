#include "pch.h"

#include "Core/Public/Time.h"
#include "HAL/Private/PlatformTime.h"

double Time::startSeconds = 0.0;
double Time::previousSeconds = 0.0;
double Time::currentSeconds = 0.0;
double Time::deltaSeconds = 0.0;

bool Time::Init()
{
	if (PlatformTime::Init() != true)
	{
		return false;
	}

	startSeconds = PlatformTime::NowSeconds();
	previousSeconds = startSeconds;
	currentSeconds = startSeconds;
	deltaSeconds = 0.0;

	return true;
}

void Time::Tick()
{
	previousSeconds = currentSeconds;
	currentSeconds = PlatformTime::NowSeconds();

	deltaSeconds = currentSeconds - previousSeconds;
}

double Time::NowSeconds()
{
	return PlatformTime::NowSeconds();
}

double Time::EngineElapsedSeconds()
{
	return currentSeconds - startSeconds;
}

double Time::DeltaSeconds()
{
	return deltaSeconds;
}

CSystemDateTime Time::LocalSystemDateTime()
{
	return PlatformTime::LocalSystemDateTime();
}

CSystemDateTime Time::UtcSystemDateTime()
{
	return PlatformTime::UtcSystemDateTime();
}
