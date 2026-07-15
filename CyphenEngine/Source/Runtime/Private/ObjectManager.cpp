#include "pch.h"

#include "Runtime/Public/ObjectManager.h"

std::vector<Object*> ObjectManager::objects;
std::queue<Object*> ObjectManager::destroyQueue;
std::queue<uint32> ObjectManager::retiredUIDs;

uint32 ObjectManager::nextUID = 1;

Object* ObjectManager::FindObject(ObjectHandle objectHandle)
{
	for (Object* object : objects)
	{
		if (object->GetHandle() == objectHandle)
		{
			return object;
		}
	}

	return nullptr;
}

void ObjectManager::CollectDestroyedObjects()
{
	while (destroyQueue.empty() == false)
	{
		Object* destroyedObject = destroyQueue.front();
		destroyQueue.pop();

		const uint32 releasedUID =
			destroyedObject->GetHandle().GetUID();

		delete destroyedObject;

		retiredUIDs.push(releasedUID);
	}
}

void ObjectManager::Shutdown()
{
	while (objects.empty() == false)
	{
		objects.back()->Destroy();
	}

	CollectDestroyedObjects();
}
