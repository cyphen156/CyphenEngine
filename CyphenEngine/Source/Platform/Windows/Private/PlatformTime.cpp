#include "pch.h"

#include "HAL/Private/PlatformTime.h"

static LARGE_INTEGER gFrequency = {};
static LARGE_INTEGER gBaseCounter = {};
static ULONGLONG gBaseTickCount = 0;
static double gSecondsPerCounter = 0.0;
static bool gUseHighResolutionTimer = false;

bool PlatformTime::Init()
{
	gBaseTickCount = GetTickCount64();

	gUseHighResolutionTimer = QueryPerformanceFrequency(&gFrequency) == TRUE;

	if (gUseHighResolutionTimer == true)
	{
		if (QueryPerformanceCounter(&gBaseCounter) == FALSE)
		{
			return false;
		}

		gSecondsPerCounter = 1.0 / static_cast<double>(gFrequency.QuadPart);

		return true;
	}

	return true;
}

/// <summary>
/// 플랫폼 단조 타이머 값을 초 단위로 반환합니다.
/// 반환값은 현재 날짜/시각이 아니며, 플랫폼 내부 기준점 이후 증가하는 시간입니다.
/// 기준점은 정밀도 보존을 위한 내부 값이며, 엔진 시작 시간 정책은 Time이 관리합니다.
/// </summary>
/// <returns>플랫폼 기준 단조 증가 시간입니다.</returns>
double PlatformTime::MonotonicSeconds()
{
	if (gUseHighResolutionTimer == true)
	{
		LARGE_INTEGER counter = {};
		QueryPerformanceCounter(&counter);

		LONGLONG elapsedCounter = counter.QuadPart - gBaseCounter.QuadPart;

		return static_cast<double>(elapsedCounter) * gSecondsPerCounter;
	}

	ULONGLONG tickCount = GetTickCount64();
	ULONGLONG elapsedTick = tickCount - gBaseTickCount;

	return static_cast<double>(elapsedTick) / 1000.0;
}

CDateTime PlatformTime::LocalDateTime()
{
	SYSTEMTIME systemTime = {};
	GetLocalTime(&systemTime);

	CDateTime result = {};
	result.year = static_cast<int>(systemTime.wYear);
	result.month = static_cast<int>(systemTime.wMonth);
	result.day = static_cast<int>(systemTime.wDay);
	result.hour = static_cast<int>(systemTime.wHour);
	result.minute = static_cast<int>(systemTime.wMinute);
	result.second = static_cast<int>(systemTime.wSecond);
	result.millisecond = static_cast<int>(systemTime.wMilliseconds);

	return result;
}

CDateTime PlatformTime::UtcDateTime()
{
	SYSTEMTIME systemTime = {};
	GetSystemTime(&systemTime);

	CDateTime result = {};
	result.year = static_cast<int>(systemTime.wYear);
	result.month = static_cast<int>(systemTime.wMonth);
	result.day = static_cast<int>(systemTime.wDay);
	result.hour = static_cast<int>(systemTime.wHour);
	result.minute = static_cast<int>(systemTime.wMinute);
	result.second = static_cast<int>(systemTime.wSecond);
	result.millisecond = static_cast<int>(systemTime.wMilliseconds);

	return result;
}
