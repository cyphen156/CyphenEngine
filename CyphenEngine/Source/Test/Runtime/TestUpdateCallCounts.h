#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// TestUpdateCallCounts
// ----------------------------------------------------------------------------
// 테스트 객체 한 인스턴스의 네 Update 실행 횟수를 기록하는 POD 값입니다.
//
// 상속 계층에는 참여하지 않습니다.
// TestRuntimeObject_*, TestWorldObject_*, TestComponent_*가 멤버로 보유하며,
// 선언하지 않은 실행 단계가 잘못 호출된 경우까지 동일한 형태로 검증합니다.
// ============================================================================

struct TestUpdateCallCounts
{
	uint32 globalUpdate = 0;
	uint32 update = 0;
	uint32 finalUpdate = 0;
	uint32 globalFinalUpdate = 0;
};

