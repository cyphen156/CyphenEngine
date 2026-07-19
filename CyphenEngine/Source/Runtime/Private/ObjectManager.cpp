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

	// SubObject의 virtual Destroy를 호출하여 각 타입에 귀속된 관계를
	// 먼저 정리한 뒤 공통 Object 파괴 경로에 합류하도록 합니다.
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
