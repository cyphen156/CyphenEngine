#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// UpdateParticipation
// ----------------------------------------------------------------------------
// GameObject와 Component가 제공하는 Update 실행 단계를 나타냅니다.
//
// 참여 선언은 객체 생성 시 확정되며 실행 중 변경하지 않습니다.
// 선언은 실행 능력을 나타낼 뿐 실제 Function Group 등록을 보장하지 않습니다.
//
// GlobalUpdate / GlobalFinalUpdate:
//   - 소속 Runtime이 확정된 객체군에서 등록합니다.
//
// Update / FinalUpdate:
//   - World::Join이 전달받은 WorldObject 본인의 참여를 등록합니다.
//
// 현재는 자식 GameObject와 Component의 참여를 자동으로 등록하지 않습니다.
// Attach / Detach에 의한 참여 변경은 후속 구현합니다.
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
