#include "pch.h"

#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

HandleAllocator<ObjectHandle> ObjectManager::handleAllocator;

std::unordered_map<uint32, Object*> ObjectManager::objects;
std::queue<Object*> ObjectManager::destroyQueue;

Object* ObjectManager::FindObject(ObjectHandle objectHandle)
{
	if (objectHandle.IsSet() == false)
	{
		return nullptr;
	}

	std::unordered_map<uint32, Object*>::iterator iterator = objects.find(objectHandle.GetValue());

	if (iterator == objects.end())
	{
		return nullptr;
	}

	return iterator->second;
}

bool ObjectManager::DestroyObject(Object* object)
{
	if (object == nullptr)
	{
		return false;
	}

	const ObjectHandle objectHandle = object->GetHandle();

	if (objectHandle.IsSet() == false)
	{
		return false;
	}

	std::unordered_map<uint32, Object*>::iterator iterator = objects.find(objectHandle.GetValue());

	if (iterator == objects.end() || iterator->second != object)
	{
		return false;
	}

	// SubObject를 부모보다 먼저 파괴합니다.
	//
	// Component는 독립적인 실행 문맥을 가질 수 없으므로
	// 부모 GameObject의 DetachSubObject를 통해 Update 참여를 해제해야 합니다.
	//
	// 자식이 먼저 파괴되면 부모 GameObject의 Runtime 및 World 문맥이
	// 아직 유지된 상태에서 Component 실행 참여를 안전하게 해제할 수 있습니다.
	while (object->subObjects.empty() == false)
	{
		Object* subObject = object->subObjects.back();

		if (subObject == nullptr)
		{
			return false;
		}

		if (subObject->Destroy() == false)
		{
			return false;
		}
	}

	// 모든 종속 SubObject가 자신의 구체 계약을 정리한 뒤
	// 현재 객체 자신의 Runtime 및 World 계약을 정리합니다.
	if (object->OnDestroy() == false)
	{
		return false;
	}

	// 파괴되는 객체가 다른 Object의 SubObject라면
	// 부모가 제공하는 관계 해제 경로를 사용합니다.
	//
	// Component라면 GameObject::DetachSubObject가 해당 Component의
	// UpdateFunction을 실행그룹에서 제거합니다.
	//
	// GameObject와 일반 Object라면 별도의 실행 계약 변경 없이
	// 구조적 관계만 해제합니다.
	if (object->outer != nullptr && object->outer->DetachSubObject(*object) == false)
	{
		return false;
	}

	objects.erase(iterator);
	destroyQueue.push(object);

	return true;
}

void ObjectManager::Collect()
{
	while (destroyQueue.empty() == false)
	{
		Object* destroyedObject = destroyQueue.front();
		destroyQueue.pop();

		const ObjectHandle releasedHandle = destroyedObject->GetHandle();

		delete destroyedObject;

		handleAllocator.Deallocate(releasedHandle);
	}
}

bool ObjectManager::Clear()
{
	while (objects.empty() == false)
	{
		std::unordered_map<uint32, Object*>::iterator iterator = objects.begin();

		if (iterator->second->Destroy() == false)
		{
			return false;
		}
	}

	Collect();

	return true;
}
