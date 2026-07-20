#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// UpdateParticipation
// ----------------------------------------------------------------------------
// GameObject와 Component가 참여할 Update 실행 단계를 나타냅니다.
//
// 참여 정보는 객체 생성 시 확정되며, 실행 중 변경하지 않습니다.
// 각 비트는 대응하는 실행 단계의 Scheduler 등록 의사를 나타냅니다.
//
// 실제 Scheduler 등록과 실행 목록 구성은 별도의 실행 정책이 담당합니다.
// ============================================================================

enum class UpdateParticipation : uint8
{
	None = 0,

	GlobalUpdate = 1 << 0,
	Update = 1 << 1,
	FinalUpdate = 1 << 2,
	GlobalFinalUpdate = 1 << 3
};

constexpr UpdateParticipation operator|(UpdateParticipation left, UpdateParticipation right)
{
	return static_cast<UpdateParticipation>(static_cast<uint8>(left) | static_cast<uint8>(right));
}
