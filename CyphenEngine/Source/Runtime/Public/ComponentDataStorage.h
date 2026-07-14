#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/Handle.h"

// ============================================================================
// ComponentDataStorage
// ----------------------------------------------------------------------------
// ComponentDataType 원소를 연속된 슬롯에 저장하고, 세대형 Handle로
// 슬롯의 논리적 수명을 관리합니다.
//
// 저장 타입은 값, 객체, 포인터 또는 Handle로 제한하지 않습니다.
// Storage는 전달받은 원소 자체만 보관하며, 포인터나 Handle이 가리키는
// 외부 대상의 소유권과 수명은 관리하지 않습니다.
//
// 원소 배열의 물리적 연속성은 보장하지만, 생존 원소의 조밀한 배치,
// 접근 순서의 연속성, 원소 주소의 안정성과 성능 향상은 보장하지 않습니다.
//
// Remove와 Clear는 슬롯의 논리적 등록을 제거하지만, 원소의 즉시 소멸이나
// 외부 자원의 해제를 보장하지 않습니다. 즉시 소멸이 필요한 객체의 소유는
// ObjectStorage와 같은 별도 수명 관리 저장소가 담당합니다.
//
// 책임:
//   - 전달받은 원소의 연속 슬롯 저장
//   - 세대형 Handle 발급과 유효성 검증
//   - 제거된 슬롯의 추적과 재사용
//
// 비책임:
//   - 외부 객체와 자원의 생성·파괴
//   - 포인터와 Handle이 가리키는 대상의 수명 관리
//   - WorldObject 생성과 수명 관리
//   - Component 부착 정책
//   - System enroll과 순회 최적화
//   - 실행 phase와 write ownership 통제
//
// 조회 결과로 내부 포인터나 참조를 노출하지 않습니다.
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

	Handle<ComponentDataType> Insert(const ComponentDataType& inElement);
	bool Remove(Handle<ComponentDataType> inHandle);

	bool IsValid(Handle<ComponentDataType> inHandle) const;

	bool TryGet(
		Handle<ComponentDataType> inHandle,
		ComponentDataType& outElement) const;

	bool Set(
		Handle<ComponentDataType> inHandle,
		const ComponentDataType& inElement);

	void Clear();

	uint32 Count() const;

private:
	bool ResolveIndex(
		Handle<ComponentDataType> inHandle,
		uint32& outIndex) const;

	void AdvanceGeneration(uint32 index);

	std::vector<ComponentDataType> elements;
	std::vector<uint32> slotGenerations;
	std::vector<uint8> slotOccupied;
	std::vector<uint32> freeSlotIndices;

	uint32 activeCount = 0;
};

template<typename ComponentDataType>
Handle<ComponentDataType>
ComponentDataStorage<ComponentDataType>::Insert(
	const ComponentDataType& inElement)
{
	uint32 index = 0;

	if (freeSlotIndices.empty() == false)
	{
		index = freeSlotIndices.back();

		// 대입에 실패하면 해당 index는 계속 빈 슬롯으로 남습니다.
		elements[index] = inElement;

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

		elements.push_back(inElement);
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
bool ComponentDataStorage<ComponentDataType>::Remove(
	Handle<ComponentDataType> inHandle)
{
	uint32 index = 0;

	if (ResolveIndex(inHandle, index) == false)
	{
		return false;
	}

	// free-list 확장이 실패하면 기존 슬롯은 계속 유효한 상태로 남습니다.
	freeSlotIndices.push_back(index);

	slotOccupied[index] = 0;
	AdvanceGeneration(index);
	--activeCount;
	return true;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::IsValid(
	Handle<ComponentDataType> inHandle) const
{
	uint32 index = 0;
	return ResolveIndex(inHandle, index);
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::TryGet(
	Handle<ComponentDataType> inHandle,
	ComponentDataType& outElement) const
{
	uint32 index = 0;

	if (ResolveIndex(inHandle, index) == false)
	{
		return false;
	}

	outElement = elements[index];
	return true;
}

template<typename ComponentDataType>
bool ComponentDataStorage<ComponentDataType>::Set(
	Handle<ComponentDataType> inHandle,
	const ComponentDataType& inElement)
{
	uint32 index = 0;

	if (ResolveIndex(inHandle, index) == false)
	{
		return false;
	}

	elements[index] = inElement;
	return true;
}

template<typename ComponentDataType>
void ComponentDataStorage<ComponentDataType>::Clear()
{
	// reserve에 실패하면 기존 슬롯 상태와 free-list를 유지합니다.
	freeSlotIndices.reserve(elements.size());
	freeSlotIndices.clear();

	for (uint32 index = 0;
		index < static_cast<uint32>(elements.size());
		++index)
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
bool ComponentDataStorage<ComponentDataType>::ResolveIndex(
	Handle<ComponentDataType> inHandle,
	uint32& outIndex) const
{
	if (inHandle.IsSet() == false)
	{
		return false;
	}

	if (inHandle.index >= elements.size())
	{
		return false;
	}

	if (slotOccupied[inHandle.index] == 0)
	{
		return false;
	}

	if (slotGenerations[inHandle.index] != inHandle.generation)
	{
		return false;
	}

	outIndex = inHandle.index;
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
