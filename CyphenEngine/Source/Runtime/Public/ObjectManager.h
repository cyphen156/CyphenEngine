#pragma once

#include <queue>
#include <unordered_map>
#include <utility>

#include "Core/Public/Handle.h"
#include "Core/Public/HandleAllocator.h"

class CyphenEngine;
class Object;

using ObjectHandle = Handle<Object>;

// ============================================================================
// ObjectManager
// ----------------------------------------------------------------------------
// 엔진 Object의 Handle 발급, 메모리 소유, Registry와 지연 파괴를 관리하는
// 정적 전역 관리 경계입니다.
//
// NewObject:
//   - HandleAllocator를 통해 ObjectHandle을 발급합니다.
//   - 발급한 ObjectHandle을 주입하여 구체 Object를 생성합니다.
//   - 생성된 Object의 실제 메모리와 Registry 관계를 소유합니다.
//
// FindObject:
//   - ObjectHandle을 통해 활성 Registry의 Object를 단건 조회합니다.
//   - Object 내부의 Handle을 역참조하여 순차 탐색하지 않습니다.
//
// Destroy:
//   - Object::Destroy 요청 시 Object를 활성 Registry에서 즉시 제거합니다.
//   - Registry에서 제거된 Object를 Destroy Queue에 보관합니다.
//   - Collect 안전 지점에서 실제 소멸자를 호출합니다.
//
// Collect:
//   - ObjectManager가 스스로 호출 시점을 결정하지 않습니다.
//   - 엔진이 선택한 안전 지점에서 Destroy Queue의 Object를 실제로 파괴합니다.
//   - 실제 파괴가 끝난 ObjectHandle을 HandleAllocator에 반환합니다.
//
// Clear:
//   - 활성 Registry에 남아 있는 모든 Object를 Destroy Queue로 이동합니다.
//   - Collect를 호출하여 활성 객체와 파괴 대기 객체를 모두 정리합니다.
//
// PendingDestroy 정책:
//   - 현재는 별도의 PendingDestroy 상태와 공개 조회 API를 두지 않습니다.
//   - 활성 Registry 포함 여부가 ObjectManager 전역 조회 가능 여부를 대신합니다.
//   - Destroy를 호출한 포인터를 다시 사용하지 않는 것은 User API 계약입니다.
//   - 파괴 대기 상태의 외부 관찰이나 복구가 필요해지면 명시적인 상태 정책을
//     추가할 수 있도록 Destroy Queue 경계를 유지합니다.
//
// ObjectManager는 인스턴스화되지 않으며 특정 GameRuntime이나 World가 소유하지 않습니다.
// Runtime, Editor와 Asset 등 Object가 필요한 엔진 도메인은 같은 Object Registry를
// 공유합니다.
//
// ObjectHandle은 ObjectManager Registry 안에서만 의미를 가집니다.
// ObjectHandle의 실제 조회와 수명 검증은 ObjectManager가 담당합니다.
//
// ObjectManager는 참조 관계나 도달 가능성을 추적하지 않습니다.
// 따라서 자동 수집 GC가 아니며, 명시적인 Destroy 요청을 안전 지점까지 지연하여
// 처리하는 Object 수명 관리 경계입니다.
// ============================================================================

class ObjectManager final
{
public:
	static Object* FindObject(ObjectHandle objectHandle);

	ObjectManager() = delete;
	~ObjectManager() = delete;

private:
	friend class CyphenEngine;
	friend class Object;

	template<typename ObjectType, typename... ArgumentTypes>
	static ObjectType* NewObject(ArgumentTypes&&... arguments);

	static bool DestroyObject(Object* object);

	static void Collect();
	static bool Clear();

	static HandleAllocator<ObjectHandle> handleAllocator;

	static std::unordered_map<uint32, Object*> objects;
	static std::queue<Object*> destroyQueue;
};

template<typename ObjectType, typename... ArgumentTypes>
ObjectType* ObjectManager::NewObject(ArgumentTypes&&... arguments)
{
	const ObjectHandle objectHandle = handleAllocator.Allocate();

	if (objectHandle.IsSet() == false)
	{
		return nullptr;
	}

	ObjectType* object = new ObjectType(objectHandle, std::forward<ArgumentTypes>(arguments)...);

	objects.emplace(objectHandle.GetValue(), object);

	return object;
}
