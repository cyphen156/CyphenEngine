#pragma once

#include "Core/Public/CDateTime.h"

// ============================================================================
// Time
// ----------------------------------------------------------------------------
// 엔진 공통 시간 시스템입니다.
//
// Time은 Core 내부 시간 시스템입니다.
// Runtime 사용자 공개 API는 추후 CClock / CDateTime 계층에서 제공합니다.
// 엔진 루프는 매 순회 시작 시 Time::Tick()을 호출하여 현재 루프에서 사용할
// 시간 샘플을 확정합니다.
//
// 책임:
//   - 엔진 시작 이후 경과 시간 계산
//   - 루프 간 DeltaTime 계산
//   - Local / UTC 시스템 날짜/시간 값 제공
//
// 비책임:
//   - 플랫폼 OS API 호출
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

	static double ElapsedTime();
	static double DeltaTime();

	static CDateTime LocalDateTime();
	static CDateTime UtcDateTime();

private:
	Time() = delete;
	~Time() = delete;

	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;

	Time(Time&& other) = delete;
	Time& operator=(Time&& other) = delete;

	// PlatformTime::MonotonicSeconds()에서 가져온 초 단위 단조 시간입니다.
	static double startTime;
	static double previousTime;
	static double currentTime;
	static double deltaTime;
};
