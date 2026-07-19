#pragma once

#include <limits>

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Handle
// ----------------------------------------------------------------------------
// 특정 관리 도메인의 대상을 참조하는 타입 안전한 값입니다.
//
// Type은 서로 다른 관리 도메인의 Handle을 컴파일 시점에 구분합니다.
// Handle은 내부 값과 Invalid 표현만을 가지며, 대상의 소유권이나 수명,
// 저장 위치, 발급과 재사용 정책을 관리하지 않습니다.
//
// 서로 다른 Handle Type 또는 독립된 관리 도메인의 Handle은
// 같은 내부 값을 가질 수 있습니다.
// 내부 값의 일치만으로 서로 다른 Handle 사이의 관계를 추론하지 않습니다.
//
// IsSet은 내부 값이 Invalid가 아닌지만 확인합니다.
// Handle에 대응하는 대상의 실제 생존 여부와 조회 가능 여부는
// 해당 대상을 관리하는 외부 경계가 검증합니다.
// ============================================================================

template<typename Type>
struct Handle
{
	static constexpr uint32 Invalid = std::numeric_limits<uint32>::max();

	uint32 value = Invalid;

	constexpr Handle() = default;

	explicit constexpr Handle(uint32 handleValue)
		: value(handleValue)
	{
	}

	constexpr bool IsSet() const
	{
		return value != Invalid;
	}

	constexpr uint32 GetValue() const
	{
		return value;
	}
};

template<typename Type>
constexpr bool operator==(const Handle<Type>& left, const Handle<Type>& right)
{
	return left.value == right.value;
}

template<typename Type>
constexpr bool operator!=(const Handle<Type>& left, const Handle<Type>& right)
{
	return !(left == right);
}

static_assert(sizeof(Handle<void>) == sizeof(uint32),
	"Handle must remain a tight 32-bit value wrapper.");
