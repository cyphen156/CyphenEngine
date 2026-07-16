#include "pch.h"

#include "Runtime/Public/World.h"
#include "Runtime/Public/WorldObject.h"

World::~World()
{
	Reset();
}

void World::Reset()
{
	for (WorldObject* worldObject : worldObjects)
	{
		if (worldObject != nullptr)
		{
			worldObject->world = nullptr;
		}
	}

	worldUpdates.clear();
	worldObjects.clear();
	transforms.Clear();

	simulationTick = 0;
	simulationTime = 0.0;
	worldOrigin = {};
}

void World::Tick(double deltaSeconds)
{
	if (deltaSeconds < 0.0)
	{
		return;
	}

	PreUpdate(deltaSeconds);
	ProcessAll(deltaSeconds);
	FinalUpdate(deltaSeconds);

	++simulationTick;
	simulationTime += deltaSeconds;
}

void World::PreUpdate(double deltaSeconds)
{
	const uint32 updateCount = static_cast<uint32>(worldUpdates.size());

	for (uint32 index = 0; index < updateCount; ++index)
	{
		WorldObject* object = worldUpdates[index];

		if (object == nullptr)
		{
			continue;
		}

		object->Update(deltaSeconds);
	}
}

void World::ProcessAll(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);
}

void World::FinalUpdate(double deltaSeconds)
{
	const uint32 updateCount = static_cast<uint32>(worldUpdates.size());

	for (uint32 index = 0; index < updateCount; ++index)
	{
		WorldObject* object = worldUpdates[index];

		if (object == nullptr)
		{
			continue;
		}

		object->FinalUpdate(deltaSeconds);
	}
}

uint64 World::GetSimulationTick() const
{
	return simulationTick;
}

double World::GetSimulationTime() const
{
	return simulationTime;
}

bool World::Enroll(WorldObject& object, const Transform& initialTransform)
{
	if (object.world != nullptr)
	{
		return false;
	}

	const StorageSlot<Transform> transformSlot = transforms.Insert(
		object.GetHandle(),
		initialTransform);

	if (transformSlot.IsSet() == false)
	{
		return false;
	}

	worldObjects.push_back(&object);

	if (object.participatesInWorldUpdate)
	{
		worldUpdates.push_back(&object);
	}

	object.world = this;
	return true;
}

bool World::Unroll(WorldObject& object)
{
	std::vector<WorldObject*>::iterator iterator;
	for (iterator = worldObjects.begin();
		iterator != worldObjects.end();
		++iterator)
	{
		if (*iterator != &object)
		{
			continue;
		}

		transforms.Remove(object.GetHandle());
		worldObjects.erase(iterator);

		std::vector<WorldObject*>::iterator updateIterator;
		for (updateIterator = worldUpdates.begin();
			updateIterator != worldUpdates.end();
			++updateIterator)
		{
			if (*updateIterator != &object)
			{
				continue;
			}

			worldUpdates.erase(updateIterator);
			break;
		}

		object.world = nullptr;
		return true;
	}

	object.world = nullptr;
	return false;
}

bool World::TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const
{
	return transforms.TryGet(objectHandle, outTransform);
}
