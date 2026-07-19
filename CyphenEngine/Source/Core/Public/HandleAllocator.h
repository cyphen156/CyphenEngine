#pragma once

#include <queue>

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// HandleAllocator
// ----------------------------------------------------------------------------
// 특정 Handle 도메인의 값을 발급하고 회수합니다.
//
// HandleAllocator 인스턴스 하나는 독립된 Handle 값 공간을 나타냅니다.
// 하나의 상호 운용 범위에서 같은 HandleType은 하나의 HandleAllocator가 관리합니다.
//
// Handle은 자신을 발급한 HandleAllocator 정보를 보관하지 않습니다.
// 따라서 HandleAllocator는 다른 Allocator가 발급한 값이나 동일한 값의 중복 반환을
// 자체적으로 검증하지 않으며, 올바른 Allocator에 한 번만 반환하는 것은
// 해당 Allocator를 소유한 Manager의 계약입니다.
//
// Allocate:
//   - 새로운 값 공간이 남아 있는 동안에는 단조 증가 값을 발급합니다.
//   - 값 공간이 소진된 경우에만 deallocatedValues에서 값을 재사용합니다.
//
// Deallocate:
//   - 실제 관리 대상의 소멸이 완료된 이후 Handle 값을 회수합니다.
// ============================================================================

template<typename HandleType>
class HandleAllocator final
{
public:
	HandleAllocator() = default;
	~HandleAllocator() = default;

	HandleAllocator(const HandleAllocator&) = delete;
	HandleAllocator& operator=(const HandleAllocator&) = delete;
	HandleAllocator(HandleAllocator&&) = delete;
	HandleAllocator& operator=(HandleAllocator&&) = delete;

	HandleType Allocate();
	void Deallocate(HandleType handle);

private:
	uint32 nextValue = 1;
	std::queue<uint32> deallocatedValues;
};

template<typename HandleType>
HandleType HandleAllocator<HandleType>::Allocate()
{
	if (nextValue != HandleType::Invalid)
	{
		return HandleType(nextValue++);
	}

	if (deallocatedValues.empty())
	{
		return HandleType();
	}

	const uint32 handleValue = deallocatedValues.front();
	deallocatedValues.pop();

	return HandleType(handleValue);
}

template<typename HandleType>
void HandleAllocator<HandleType>::Deallocate(HandleType handle)
{
	if (handle.IsSet() == false)
	{
		return;
	}

	deallocatedValues.push(handle.GetValue());
}
