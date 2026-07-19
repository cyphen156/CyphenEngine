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

void ObjectManager::DestroyObject(Object* object)
{
	if (object == nullptr)
	{
		return;
	}

	const ObjectHandle objectHandle = object->GetHandle();

	if (objectHandle.IsSet() == false)
	{
		return;
	}

	std::unordered_map<uint32, Object*>::iterator iterator = objects.find(objectHandle.GetValue());

	if (iterator == objects.end())
	{
		return;
	}

	if (iterator->second != object)
	{
		return;
	}

	destroyQueue.push(object);
	objects.erase(iterator);
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

void ObjectManager::Clear()
{
	while (objects.empty() == false)
	{
		std::unordered_map<uint32, Object*>::iterator iterator = objects.begin();

		Object* object = iterator->second;

		destroyQueue.push(object);
		objects.erase(iterator);
	}

	Collect();
}
