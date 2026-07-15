#pragma once

#include <queue>
#include <utility>
#include <vector>

#include "Runtime/Public/Object.h"

class CyphenEngine;

// ============================================================================
// ObjectManager
// ----------------------------------------------------------------------------
// 엔진 Object의 UID 발급, 메모리 소유, Registry와 지연 파괴를 관리하는
// 정적 전역 관리 경계입니다.
//
// NewObject:
//   - ObjectHandle을 발급하고 구체 Object를 생성합니다.
//   - 생성된 Object의 실제 메모리와 Registry record를 소유합니다.
//
// Destroy:
//   - Object::Destroy 요청 시 Object를 활성 Registry에서 즉시 제거합니다.
//   - Registry에서 제거된 Object를 Destroy Queue에 보관합니다.
//   - CollectDestroyedObjects 안전 지점에서 실제 소멸자를 호출합니다.
//
// PendingDestroy 정책:
//   - 현재는 별도의 PendingDestroy 상태와 공개 조회 API를 두지 않습니다.
//   - 활성 Registry 포함 여부가 ObjectManager 전역 조회 가능 여부를 대신합니다.
//   - Destroy를 호출한 포인터를 다시 사용하지 않는 것은 User API 계약입니다.
//   - 파괴 대기 상태의 외부 관찰이나 복구가 필요해지면 명시적인 상태 정책을
//     추가할 수 있도록 Destroy Queue 경계를 유지합니다.
//
// ObjectManager는 인스턴스화되지 않으며 특정 GameRuntime이나 World가 소유하지 않습니다.
// Runtime, Editor와 Asset 등 Object가 필요한 모든 엔진 도메인이 같은 UID 원천과
// Registry를 공유합니다.
//
// 소멸된 UID는 retired Queue에 수집합니다. 단조 UID 공간이 남아 있는 동안에는
// 새 UID를 우선 발급하고, 공간이 소진된 경우에만 retired UID를 재사용합니다.
// ============================================================================

class ObjectManager final
{
public:
	template<typename ObjectType, typename... ArgumentTypes>
	static ObjectType* NewObject(ArgumentTypes&&... arguments);

	static Object* FindObject(ObjectHandle objectHandle);

	ObjectManager() = delete;
	~ObjectManager() = delete;

private:
	friend class CyphenEngine;
	friend class Object;

	static void CollectDestroyedObjects();
	static void Shutdown();

	static std::vector<Object*> objects;
	static std::queue<Object*> destroyQueue;
	static std::queue<uint32> retiredUIDs;

	static uint32 nextUID;
};

template<typename ObjectType, typename... ArgumentTypes>
ObjectType* ObjectManager::NewObject(ArgumentTypes&&... arguments)
{
	uint32 uid = ObjectHandle::InvalidUID;

	if (nextUID != ObjectHandle::InvalidUID)
	{
		uid = nextUID++;
	}
	else
	{
		if (retiredUIDs.empty())
		{
			return nullptr;
		}

		uid = retiredUIDs.front();
		retiredUIDs.pop();
	}

	const ObjectHandle objectHandle(uid);

	if (objectHandle.IsSet() == false)
	{
		return nullptr;
	}

	ObjectType* object = new ObjectType(
		objectHandle,
		std::forward<ArgumentTypes>(arguments)...);

	objects.push_back(object);

	return object;
}
