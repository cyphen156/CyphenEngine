#include "pch.h"

#include "HAL/Private/PlatformTime.h"

static LARGE_INTEGER gFrequency = {};
static bool gUseHighResolutionTimer = false;

bool PlatformTime::Init()
{
	gUseHighResolutionTimer = QueryPerformanceFrequency(&gFrequency) == TRUE;
	return true;
}

double PlatformTime::NowSeconds()
{
	if (gUseHighResolutionTimer)
	{
		LARGE_INTEGER counter = {};
		QueryPerformanceCounter(&counter);
		return static_cast<double>(counter.QuadPart) / static_cast<double>(gFrequency.QuadPart);
	}
	return static_cast<double>(GetTickCount64()) / 1000.0;
}

CSystemDateTime PlatformTime::LocalSystemDateTime()
{
	SYSTEMTIME systemTime = {};
	GetLocalTime(&systemTime);
	CSystemDateTime result = {};
	result.year = systemTime.wYear;
	result.month = systemTime.wMonth;
	result.day = systemTime.wDay;
	result.hour = systemTime.wHour;
	result.minute = systemTime.wMinute;
	result.second = systemTime.wSecond;
	result.millisecond = systemTime.wMilliseconds;
	return result;
}

CSystemDateTime PlatformTime::UtcSystemDateTime()
{
	SYSTEMTIME systemTime = {};
	GetSystemTime(&systemTime);
	CSystemDateTime result = {};
	result.year = systemTime.wYear;
	result.month = systemTime.wMonth;
	result.day = systemTime.wDay;
	result.hour = systemTime.wHour;
	result.minute = systemTime.wMinute;
	result.second = systemTime.wSecond;
	result.millisecond = systemTime.wMilliseconds;
	return result;
}
