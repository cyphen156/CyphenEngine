#pragma once

#include <limits>

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Handle
// ----------------------------------------------------------------------------
// 런타임 인스턴스의 UID를 타입 안전하게 전달하는 강타입 래퍼입니다.
//
// Type은 서로 다른 도메인의 Handle을 컴파일 시점에 구분합니다.
// Handle은 저장 위치나 재사용 세대를 표현하지 않으며, 실제 조회와 수명 검증은
// 해당 인스턴스를 관리하는 경계가 담당합니다.
// ============================================================================

template<typename Type>
struct Handle
{
	static constexpr uint32 InvalidUID = std::numeric_limits<uint32>::max();

	uint32 uid = InvalidUID;

	constexpr Handle() = default;

	explicit constexpr Handle(uint32 uidValue)
		: uid(uidValue)
	{
	}

	constexpr bool IsSet() const
	{
		return uid != InvalidUID;
	}

	constexpr uint32 GetUID() const
	{
		return uid;
	}

	constexpr void Release()
	{
		uid = InvalidUID;
	}
};

template<typename Type>
constexpr bool operator==(const Handle<Type>& left, const Handle<Type>& right)
{
	return left.uid == right.uid;
}

template<typename Type>
constexpr bool operator!=(const Handle<Type>& left, const Handle<Type>& right)
{
	return !(left == right);
}

static_assert(sizeof(Handle<void>) == sizeof(uint32),
	"Handle must remain a tight 32-bit UID wrapper.");
