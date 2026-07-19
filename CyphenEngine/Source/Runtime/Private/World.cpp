#include "pch.h"

#include "Runtime/Public/World.h"
#include "Runtime/Public/WorldObject.h"

World::~World()
{
	Reset();
}

bool World::Join(WorldObject& worldObject, const Transform& initialTransform)
{
	if (worldObject.world != nullptr)
	{
		return false;
	}

	const StorageSlot<Transform> transformSlot = transforms.Insert(worldObject.GetHandle(), initialTransform);

	if (transformSlot.IsSet() == false)
	{
		return false;
	}

	worldObjects.push_back(&worldObject);
	worldObject.world = this;

	return true;
}

bool World::Leave(WorldObject& worldObject)
{
	if (worldObject.world != this)
	{
		return false;
	}

	std::vector<WorldObject*>::iterator iterator;
	for (iterator = worldObjects.begin(); iterator != worldObjects.end(); ++iterator)
	{
		if (*iterator == &worldObject)
		{
			break;
		}
	}

	if (iterator == worldObjects.end())
	{
		return false;
	}

	if (transforms.Remove(worldObject.GetHandle()) == false)
	{
		return false;
	}

	worldObjects.erase(iterator);
	worldObject.world = nullptr;

	return true;
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

	PreUpdate();
	ProcessAll(deltaSeconds);
	FinalUpdate();

	++simulationTick;
	simulationTime += deltaSeconds;
}

void World::PreUpdate()
{
	// 실행 참여 대상과 Scheduler 계약을 확정한 이후 구현합니다.
	// World는 현재 선행 실행 단계의 경계만 보장합니다.
}

void World::ProcessAll(double deltaSeconds)
{
	// World-local System 참여와 실행 순서를 확정한 이후 구현합니다.
	// 현재는 Process 단계의 경계와 delta time 전달 계약만 유지합니다.
	static_cast<void>(deltaSeconds);
}

void World::FinalUpdate()
{
	// 실행 참여 대상과 Scheduler 계약을 확정한 이후 구현합니다.
	// World는 현재 후행 실행 단계의 경계만 보장합니다.
}

uint64 World::GetSimulationTick() const
{
	return simulationTick;
}

double World::GetSimulationTime() const
{
	return simulationTime;
}

bool World::TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const
{
	return transforms.TryGet(objectHandle, outTransform);
}
