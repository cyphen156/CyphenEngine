#pragma once

#include <limits>

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// StorageSlot
// ----------------------------------------------------------------------------
// Storage의 재사용 가능한 슬롯을 안전하게 참조하는 로컬 접근 토큰입니다.
//
// index는 Storage 내부 위치이고, generation은 슬롯 재사용 전후의 오래된 참조를
// 구분합니다. 실제 유효성은 소유 Storage가 두 값을 함께 검증합니다.
// ============================================================================

template<typename Type>
struct StorageSlot
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
constexpr bool operator==(const StorageSlot<Type>& left, const StorageSlot<Type>& right)
{
	return left.index == right.index && left.generation == right.generation;
}

template<typename Type>
constexpr bool operator!=(const StorageSlot<Type>& left, const StorageSlot<Type>& right)
{
	return !(left == right);
}

static_assert(sizeof(StorageSlot<void>) == sizeof(uint32) * 2,
	"StorageSlot must remain tight index and generation storage.");
