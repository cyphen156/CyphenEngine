#pragma once

#include "Core/Public/CSystemDateTime.h"

// ============================================================================
// Time
// ----------------------------------------------------------------------------
// 엔진 공통 시간 시스템입니다.
//
// Time은 외부에 공개되는 정적 시간 API입니다.
// 엔진 루프는 매 순회 시작 시 Time::Tick()을 호출하여 현재 루프에서 사용할
// 시간 샘플을 확정합니다.
//
// 책임:
//   - 엔진 시작 이후 경과 시간 계산
//   - 루프 간 DeltaSeconds 계산
//   - 현재 고해상도 시간 질의 래핑
//   - Local / UTC 시스템 날짜/시간 값 제공
//
// 비책임:
//   - CString 포맷
//   - FPS 계산
//   - FixedUpdate accumulator
//   - FrameLimiter
//   - TimeScale
//   - Event / Timer Queue
//
// 플랫폼별 OS 시간 획득은 PlatformTime이 담당하며,
// Time은 PlatformTime을 외부에 노출하지 않고 래핑해서 제공합니다.
// ============================================================================

class Time final
{
public:
	static bool Init();
	static void Tick();

	static double NowSeconds();
	static double EngineElapsedSeconds();
	static double DeltaSeconds();

	static CSystemDateTime LocalSystemDateTime();
	static CSystemDateTime UtcSystemDateTime();

private:
	Time() = delete;
	~Time() = delete;

	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;

	Time(Time&& other) = delete;
	Time& operator=(Time&& other) = delete;

private:
	static double startSeconds;
	static double previousSeconds;
	static double currentSeconds;
	static double deltaSeconds;
};
