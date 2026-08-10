#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// UpdateParticipation
// ----------------------------------------------------------------------------
// GameObject와 Component가 제공하는 Update 실행 단계를 나타냅니다.
//
// 참여 선언은 객체 생성 시 확정되며 실행 중 변경하지 않는 등록 의사입니다.
// 실제 Function Group 등록 여부와 현재 실행 상태는 Runtime / World 소속과
// 활성 정책이 별도로 결정합니다.
//
// GlobalUpdate / GlobalFinalUpdate:
//   - GameRuntime::Admit이 서브트리의 각 GameObject와 직접 Component의 참여를
//     Runtime-global Function Group에 등록합니다.
//
// Update / FinalUpdate:
//   - Outer 체인의 최초 WorldObject를 World anchor로 사용합니다.
//   - anchor가 World에 합류한 경우에만 해당 World의 Function Group에 등록합니다.
//   - World::Join이 anchor 실행 범위의 참여를 일괄 등록하고,
//     이후의 Attach는 Admit 경로에서 개별 등록합니다.
//
// Component의 참여는 직접 부모 GameObject가 공급하고 등록과 해제를 대행합니다.
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
