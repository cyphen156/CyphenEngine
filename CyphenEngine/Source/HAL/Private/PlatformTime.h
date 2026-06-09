#pragma once

#include "Core/Public/CSystemDateTime.h"

class Time;

// ============================================================================
// PlatformTime
// ----------------------------------------------------------------------------
// Time이 내부에서 사용하는 플랫폼 시간 HAL 부품입니다.
//
// 이 클래스는 외부 공개 API가 아닙니다.
// OS별 시간 획득 방식을 숨기고, Time이 사용할 수 있는 공통 값으로 변환합니다.
//
// 책임:
//   - 고해상도 단조 시간 획득
//   - 현재 Local 시스템 날짜/시간 획득
//   - 현재 UTC 시스템 날짜/시간 획득
//
// 비책임:
//   - DeltaSeconds 계산
//   - EngineElapsedSeconds 계산
//   - CString 포맷
//   - FPS / FixedUpdate / FrameLimiter 정책
//
// 접근 제어:
//   - 모든 함수는 private static입니다.
//   - Time만 friend로 접근할 수 있습니다.
//   - 외부 코드는 PlatformTime을 직접 호출하지 않고 Time을 사용해야 합니다.
// ============================================================================

class PlatformTime final
{
	friend class Time;

private:
	PlatformTime() = delete;
	~PlatformTime() = delete;

	PlatformTime(const PlatformTime& other) = delete;
	PlatformTime& operator=(const PlatformTime& other) = delete;

	PlatformTime(PlatformTime&& other) = delete;
	PlatformTime& operator=(PlatformTime&& other) = delete;

private:
	static bool Init();
	static double NowSeconds();

	static CSystemDateTime LocalSystemDateTime();
	static CSystemDateTime UtcSystemDateTime();
};
