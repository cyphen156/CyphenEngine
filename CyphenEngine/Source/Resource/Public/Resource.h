#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Resource
// ----------------------------------------------------------------------------
// Engine 전체에서 공유하는 Resource 논리 핸들과 공통 분류입니다.
//
// 책임:
//   - ResourceId 공통 타입 정의
//   - ResourceKind 공통 분류 정의
//   - 구체 Resource 타입의 공통 header 제공
//
// 비책임:
//   - File I/O
//   - Codec decode
//   - GPU resource 생성 / 해제
//   - Backend 내부 slot / descriptor / API handle 관리
// ============================================================================

using ResourceId = uint64;

constexpr ResourceId InvalidResourceId = 0;

enum class ResourceKind : uint32
{
	Unknown = 0,
	Texture2D = 1
};

struct Resource
{
	ResourceId resourceId = InvalidResourceId;
	ResourceKind kind = ResourceKind::Unknown;
};
