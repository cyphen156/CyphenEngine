#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/StorageSlot.h"

// ============================================================================
// ComponentDataStorage
// ----------------------------------------------------------------------------
// ComponentDataType 요소를 연속된 슬롯에 저장하고 StorageSlot을 통해 슬롯의
// 유효성과 세대를 검증합니다. 각 슬롯은 OwnerHandleType과 연결되며, 관리 경로에서는
// owner handle로 현재 슬롯을 resolve할 수 있습니다.
//
// Storage는 전달받은 요소 자체를 값으로 보관합니다.
// 포인터나 Handle, StorageSlot이 가리키는 외부 대상의 수명은 관리하지 않습니다.
//
// 요소 배열의 물리적 연속성은 보장하지만 생존 요소의 조밀한 배치, 순회 순서,
// 요소 주소의 안정성과 성능 특성은 보장하지 않습니다.
//
// Remove와 Clear는 슬롯을 논리적으로 미사용 상태로 전환합니다.
// 요소의 즉시 소멸이나 외부 자원의 즉시 해제를 보장하지 않습니다.
// ============================================================================

template<typename ComponentDataType, typename OwnerHandleType>
class ComponentDataStorage final
{
public:
	ComponentDataStorage() = default;
	~ComponentDataStorage() = default;

	ComponentDataStorage(const ComponentDataStorage&) = delete;
	ComponentDataStorage& operator=(const ComponentDataStorage&) = delete;
	ComponentDataStorage(ComponentDataStorage&&) = delete;
	ComponentDataStorage& operator=(ComponentDataStorage&&) = delete;

	StorageSlot<ComponentDataType> Insert(
		OwnerHandleType ownerHandle,
		const ComponentDataType& element);

	bool Remove(StorageSlot<ComponentDataType> slot);
	bool Remove(OwnerHandleType ownerHandle);

	bool IsValid(StorageSlot<ComponentDataType> slot) const;

	bool TryGet(StorageSlot<ComponentDataType> slot, ComponentDataType& outElement) const;
	bool TryGet(OwnerHandleType ownerHandle, ComponentDataType& outElement) const;

	bool Set(StorageSlot<ComponentDataType> slot, const ComponentDataType& element);

	void Clear();

	uint32 Count() const;

private:
	bool ResolveIndex(StorageSlot<ComponentDataType> slot, uint32& outIndex) const;
	bool ResolveSlot(
		OwnerHandleType ownerHandle,
		StorageSlot<ComponentDataType>& outSlot) const;

	void AdvanceGeneration(uint32 index);

	std::vector<ComponentDataType> elements;
	std::vector<OwnerHandleType> ownerHandles;
	std::vector<uint32> slotGenerations;
	std::vector<uint8> slotOccupied;
	std::vector<uint32> freeSlotIndices;

	uint32 activeCount = 0;
};

template<typename ComponentDataType, typename OwnerHandleType>
StorageSlot<ComponentDataType>
ComponentDataStorage<ComponentDataType, OwnerHandleType>::Insert(
	OwnerHandleType ownerHandle,
	const ComponentDataType& element)
{
	StorageSlot<ComponentDataType> existingSlot;

	if (ownerHandle.IsSet() == false ||
		ResolveSlot(ownerHandle, existingSlot))
	{
		return {};
	}

	uint32 index = 0;

	if (freeSlotIndices.empty() == false)
	{
		index = freeSlotIndices.back();

		// 대입에 실패하면 해당 index는 계속 빈 슬롯으로 남습니다.
		elements[index] = element;
		ownerHandles[index] = ownerHandle;

		freeSlotIndices.pop_back();
		slotOccupied[index] = 1;
	}
	else
	{
		if (elements.size() >= StorageSlot<ComponentDataType>::InvalidIndex)
		{
			return {};
		}

		index = static_cast<uint32>(elements.size());

		elements.push_back(element);
		ownerHandles.push_back(ownerHandle);
		slotGenerations.push_back(1);
		slotOccupied.push_back(1);
	}

	++activeCount;

	StorageSlot<ComponentDataType> slot;
	slot.index = index;
	slot.generation = slotGenerations[index];
	return slot;
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::Remove(
	StorageSlot<ComponentDataType> slot)
{
	uint32 index = 0;

	if (ResolveIndex(slot, index) == false)
	{
		return false;
	}

	// free-list 확장에 실패하면 기존 슬롯은 계속 유효하게 남습니다.
	freeSlotIndices.push_back(index);

	ownerHandles[index] = {};
	slotOccupied[index] = 0;
	AdvanceGeneration(index);
	--activeCount;
	return true;
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::Remove(
	OwnerHandleType ownerHandle)
{
	StorageSlot<ComponentDataType> slot;

	if (ResolveSlot(ownerHandle, slot) == false)
	{
		return false;
	}

	return Remove(slot);
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::IsValid(
	StorageSlot<ComponentDataType> slot) const
{
	uint32 index = 0;
	return ResolveIndex(slot, index);
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::TryGet(
	StorageSlot<ComponentDataType> slot,
	ComponentDataType& outElement) const
{
	uint32 index = 0;

	if (ResolveIndex(slot, index) == false)
	{
		return false;
	}

	outElement = elements[index];
	return true;
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::TryGet(
	OwnerHandleType ownerHandle,
	ComponentDataType& outElement) const
{
	StorageSlot<ComponentDataType> slot;

	if (ResolveSlot(ownerHandle, slot) == false)
	{
		return false;
	}

	return TryGet(slot, outElement);
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::Set(
	StorageSlot<ComponentDataType> slot,
	const ComponentDataType& element)
{
	uint32 index = 0;

	if (ResolveIndex(slot, index) == false)
	{
		return false;
	}

	elements[index] = element;
	return true;
}

template<typename ComponentDataType, typename OwnerHandleType>
void ComponentDataStorage<ComponentDataType, OwnerHandleType>::Clear()
{
	// reserve가 실패하면 기존 슬롯 상태와 free-list를 유지합니다.
	freeSlotIndices.reserve(elements.size());
	freeSlotIndices.clear();

	for (uint32 index = 0; index < static_cast<uint32>(elements.size()); ++index)
	{
		ownerHandles[index] = {};

		if (slotOccupied[index] != 0)
		{
			slotOccupied[index] = 0;
			AdvanceGeneration(index);
		}

		freeSlotIndices.push_back(index);
	}

	activeCount = 0;
}

template<typename ComponentDataType, typename OwnerHandleType>
uint32 ComponentDataStorage<ComponentDataType, OwnerHandleType>::Count() const
{
	return activeCount;
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::ResolveIndex(
	StorageSlot<ComponentDataType> slot,
	uint32& outIndex) const
{
	if (slot.IsSet() == false)
	{
		return false;
	}

	if (slot.index >= elements.size())
	{
		return false;
	}

	if (slotOccupied[slot.index] == 0)
	{
		return false;
	}

	if (slotGenerations[slot.index] != slot.generation)
	{
		return false;
	}

	outIndex = slot.index;
	return true;
}

template<typename ComponentDataType, typename OwnerHandleType>
bool ComponentDataStorage<ComponentDataType, OwnerHandleType>::ResolveSlot(
	OwnerHandleType ownerHandle,
	StorageSlot<ComponentDataType>& outSlot) const
{
	if (ownerHandle.IsSet() == false)
	{
		return false;
	}

	const uint32 slotCount = static_cast<uint32>(ownerHandles.size());

	for (uint32 index = 0; index < slotCount; ++index)
	{
		if (slotOccupied[index] == 0 ||
			ownerHandles[index] != ownerHandle)
		{
			continue;
		}

		outSlot.index = index;
		outSlot.generation = slotGenerations[index];
		return true;
	}

	return false;
}

template<typename ComponentDataType, typename OwnerHandleType>
void ComponentDataStorage<ComponentDataType, OwnerHandleType>::AdvanceGeneration(
	uint32 index)
{
	++slotGenerations[index];

	// generation 0은 기본 미설정 상태와 구분하기 위해 사용하지 않습니다.
	if (slotGenerations[index] == 0)
	{
		slotGenerations[index] = 1;
	}
}
