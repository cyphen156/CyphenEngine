#include "pch.h"

#include <ctime>

#include "HAL/Private/PlatformTime.h"

namespace
{
	constexpr double NANOSECONDS_PER_SECOND = 1000000000.0;

	timespec gBaseMonotonic = {};

	CDateTime ToDateTime(const tm& brokenDownTime, long nanoseconds)
	{
		CDateTime result = {};
		result.year = brokenDownTime.tm_year + 1900;
		result.month = brokenDownTime.tm_mon + 1;
		result.day = brokenDownTime.tm_mday;
		result.hour = brokenDownTime.tm_hour;
		result.minute = brokenDownTime.tm_min;
		result.second = brokenDownTime.tm_sec;
		result.millisecond = static_cast<int>(nanoseconds / 1000000);

		return result;
	}

	bool ReadRealtime(timespec& outTime)
	{
		return ::clock_gettime(CLOCK_REALTIME, &outTime) == 0;
	}

	bool ReadMonotonicTime(timespec& outTime)
	{
		return ::clock_gettime(CLOCK_MONOTONIC, &outTime) == 0;
	}
}

bool PlatformTime::Init()
{
	// Linux는 CLOCK_MONOTONIC을 엔진 단조 시간 기준으로 사용합니다.
	// 기준점은 날짜/시각이 아니라 elapsed 계산용 내부 기준입니다.
	return ReadMonotonicTime(gBaseMonotonic);
}

double PlatformTime::MonotonicSeconds()
{
	timespec now = {};

	if (!ReadMonotonicTime(now))
	{
		return 0.0;
	}

	const long long elapsedSeconds =
		static_cast<long long>(now.tv_sec) -
		static_cast<long long>(gBaseMonotonic.tv_sec);

	const long elapsedNanoseconds =
		now.tv_nsec - gBaseMonotonic.tv_nsec;

	return static_cast<double>(elapsedSeconds) +
		static_cast<double>(elapsedNanoseconds) / NANOSECONDS_PER_SECOND;
}

CDateTime PlatformTime::LocalDateTime()
{
	// CLOCK_REALTIME을 1회 읽고, 같은 instant의 초/밀리초를 localtime_r로 분해합니다.
	timespec now = {};

	if (!ReadRealtime(now))
	{
		return CDateTime();
	}

	const time_t seconds = static_cast<time_t>(now.tv_sec);

	tm brokenDownTime = {};

	if (::localtime_r(&seconds, &brokenDownTime) == nullptr)
	{
		return CDateTime();
	}

	return ToDateTime(brokenDownTime, now.tv_nsec);
}

CDateTime PlatformTime::UtcDateTime()
{
	// CLOCK_REALTIME을 1회 읽고, 같은 instant의 초/밀리초를 gmtime_r로 UTC 분해합니다.
	timespec now = {};

	if (!ReadRealtime(now))
	{
		return CDateTime();
	}

	const time_t seconds = static_cast<time_t>(now.tv_sec);

	tm brokenDownTime = {};

	if (::gmtime_r(&seconds, &brokenDownTime) == nullptr)
	{
		return CDateTime();
	}

	return ToDateTime(brokenDownTime, now.tv_nsec);
}
