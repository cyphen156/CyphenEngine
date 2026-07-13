#pragma once

#include <limits>

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Handle
// ----------------------------------------------------------------------------
// Storage 슬롯을 타입 안전하게 참조하는 세대형 핸들입니다.
//
// index는 Storage의 슬롯 위치이고, generation은 슬롯 재사용 전의 오래된
// 참조를 구분합니다.
//
// Handle은 값의 설정 여부만 판단합니다.
// 실제 유효성은 소유 Storage가 index와 generation을 함께 검증합니다.
// ============================================================================

template<typename Type>
struct Handle
{
	static constexpr uint32 InvalidIndex = std::numeric_limits<uint32>::max();

	uint32 index = InvalidIndex;
	uint32 generation = 0;

	constexpr bool IsSet() const
	{
		return index != InvalidIndex;
	}
};

template<typename Type>
constexpr bool operator==(const Handle<Type>& left, const Handle<Type>& right)
{
	return left.index == right.index && left.generation == right.generation;
}

template<typename Type>
constexpr bool operator!=(const Handle<Type>& left, const Handle<Type>& right)
{
	return !(left == right);
}

static_assert(sizeof(Handle<void>) == sizeof(uint32) * 2,
	"Handle must remain tight index and generation storage.");
