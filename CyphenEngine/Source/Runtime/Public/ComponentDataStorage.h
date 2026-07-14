#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/Handle.h"

// ============================================================================
// ComponentDataStorage
// ----------------------------------------------------------------------------
// ComponentDataType 요소를 연속된 슬롯에 저장하고 Handle을 통해 슬롯의
// 유효성과 세대를 검증합니다.
//
// Storage는 전달받은 요소 자체를 값으로 보관합니다.
// 포인터나 Handle이 가리키는 외부 대상의 수명은 관리하지 않습니다.
//
// 요소 배열의 물리적 연속성은 보장하지만 생존 요소의 조밀한 배치,
// 순회 순서, 요소 주소의 안정성과 성능 향상은 보장하지 않습니다.
//
// Remove와 Clear는 슬롯의 논리적 등록을 제거합니다.
// 요소의 즉시 소멸이나 외부 자원의 즉시 해제를 보장하지 않습니다.
// ============================================================================

template<typename ComponentDataType>
class ComponentDataStorage final
{
public:
	ComponentDataStorage() = default;
	~ComponentDataStorage() = default;

	ComponentDataStorage(const ComponentDataStorage&) = delete;
	ComponentDataStorage& operator=(const ComponentDataStorage&) = delete;
	ComponentDataStorage(ComponentDataStorage&&) = delete;
	ComponentDataStorage& operator=(ComponentDataStorage&&) = delete;

	Handle<ComponentDataType> Insert(const ComponentDataType& element);

	bool Remove(Handle<ComponentDataType> handle);

	bool IsValid(Handle<ComponentDataType> handle) const;

	bool TryGet(Handle<ComponentDataType> handle, ComponentDataType& outElement) const;

	bool Set(Handle<ComponentDataType> handle, const ComponentDataType& element);

	void Clear();

	uint32 Count() const;

private:
	bool ResolveIndex(Handle<ComponentDataType> handle, uint32& outIndex) const;

	void AdvanceGeneration(uint32 index);

	std::vector<ComponentDataType> elements;
	std::vector<uint32> slotGenerations;
	std::vector<uint8> slotOccupied;
	std::vector<uint32> freeSlotIndices;

	uint32 activeCount = 0;
};

template<typename ComponentDataType>
Handle<ComponentDataType>
ComponentDataStorage<ComponentDataType>::Insert(const ComponentDataType& element)
{
	uint32 index = 0;

	if (freeSlotIndices.empty() == false)
	{
		index = freeSlotIndices.back();

		// 대입에 실패하면 해당 index는 계속 빈 슬롯으로 남습니다.
		elements[index] = element;

		freeSlotIndices.pop_back();
		slotOccupied[index] = 1;
	}
	else
	{
		if (elements.size() >= Handle<ComponentDataType>::InvalidIndex)
		{
			return {};
		}

		index = static_cast<uint32>(elements.size());

		elements.push_back(element);
		slotGenerations.push_back(1);
		slotOccupied.push_back(1);
	}

	++activeCount;

	Handle<ComponentDataType> handle;
	handle.index = index;
	handle.generation = slotGenerations[index];
	return handle;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::Remove(Handle<ComponentDataType> handle)
{
	uint32 index = 0;

	if (ResolveIndex(handle, index) == false)
	{
		return false;
	}

	// free-list 확장에 실패하면 기존 슬롯은 계속 유효하게 남습니다.
	freeSlotIndices.push_back(index);

	slotOccupied[index] = 0;
	AdvanceGeneration(index);
	--activeCount;
	return true;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::IsValid(Handle<ComponentDataType> handle) const
{
	uint32 index = 0;
	return ResolveIndex(handle, index);
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::TryGet(Handle<ComponentDataType> handle, ComponentDataType& outElement) const
{
	uint32 index = 0;

	if (ResolveIndex(handle, index) == false)
	{
		return false;
	}

	outElement = elements[index];
	return true;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::Set(Handle<ComponentDataType> handle, const ComponentDataType& element)
{
	uint32 index = 0;

	if (ResolveIndex(handle, index) == false)
	{
		return false;
	}

	elements[index] = element;
	return true;
}

template<typename ComponentDataType>
void ComponentDataStorage<ComponentDataType>::Clear()
{
	// reserve에 실패하면 기존 슬롯 상태와 free-list를 유지합니다.
	freeSlotIndices.reserve(elements.size());
	freeSlotIndices.clear();

	for (uint32 index = 0; index < static_cast<uint32>(elements.size()); ++index)
	{
		if (slotOccupied[index] != 0)
		{
			slotOccupied[index] = 0;
			AdvanceGeneration(index);
		}

		freeSlotIndices.push_back(index);
	}

	activeCount = 0;
}

template<typename ComponentDataType>
uint32 ComponentDataStorage<ComponentDataType>::Count() const
{
	return activeCount;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::ResolveIndex(Handle<ComponentDataType> handle, uint32& outIndex) const
{
	if (handle.IsSet() == false)
	{
		return false;
	}

	if (handle.index >= elements.size())
	{
		return false;
	}

	if (slotOccupied[handle.index] == 0)
	{
		return false;
	}

	if (slotGenerations[handle.index] != handle.generation)
	{
		return false;
	}

	outIndex = handle.index;
	return true;
}

template<typename ComponentDataType>
void ComponentDataStorage<ComponentDataType>::AdvanceGeneration(uint32 index)
{
	++slotGenerations[index];

	// generation 0은 기본 미설정 상태와 구분하기 위해 사용하지 않습니다.
	if (slotGenerations[index] == 0)
	{
		slotGenerations[index] = 1;
	}
}
