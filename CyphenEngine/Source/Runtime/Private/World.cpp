#include "pch.h"

#include "Runtime/Public/World.h"
#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/WorldObject.h"

// 디버그용 Tick Call Count
#if _DEBUG
uint32 updateCallCount = 0;
uint32 finalUpdateCallCount = 0;
#endif

bool World::Join(WorldObject& worldObject, const Transform& initialTransform)
{
	if (worldObject.world != nullptr || worldObject.GetGameRuntime() != &owningRuntime)
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

	if (worldObject.HasUpdateParticipation(UpdateParticipation::Update))
	{
		updateFunctions.push_back(
			{
				&worldObject,
				ExecuteUpdateFunction<GameObject, &GameObject::Update>
			});
	}

	if (worldObject.HasUpdateParticipation(UpdateParticipation::FinalUpdate))
	{
		finalUpdateFunctions.push_back(
			{
				&worldObject,
				ExecuteUpdateFunction<GameObject, &GameObject::FinalUpdate>
			});
	}

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

	const uint32 updateCount = static_cast<uint32>(updateFunctions.size());

	for (uint32 index = 0; index < updateCount; ++index)
	{
		if (updateFunctions[index].target != &worldObject)
		{
			continue;
		}

		updateFunctions[index] = updateFunctions.back();
		updateFunctions.pop_back();

		break;
	}

	const uint32 finalUpdateCount = static_cast<uint32>(finalUpdateFunctions.size());

	for (uint32 index = 0; index < finalUpdateCount; ++index)
	{
		if (finalUpdateFunctions[index].target != &worldObject)
		{
			continue;
		}

		finalUpdateFunctions[index] = finalUpdateFunctions.back();
		finalUpdateFunctions.pop_back();

		break;
	}

	worldObjects.erase(iterator);
	worldObject.world = nullptr;

	return true;
}

uint64 World::GetSimulationTick() const
{
	return simulationTick;
}

double World::GetSimulationTime() const
{
	return simulationTime;
}

const GameRuntime& World::GetGameRuntime() const
{
	return owningRuntime;
}

bool World::TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const
{
	return transforms.TryGet(objectHandle, outTransform);
}

World::World(GameRuntime& owningRuntime)
	: owningRuntime(owningRuntime)
{
}

World::~World()
{
	Reset();
}

void World::Reset()
{
	updateFunctions.clear();
	finalUpdateFunctions.clear();

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

#if _DEBUG
	updateCallCount = 0;
	finalUpdateCallCount = 0;
#endif

	Update(deltaSeconds);
	ProcessAll(deltaSeconds);
	FinalUpdate(deltaSeconds);

	++simulationTick;
	simulationTime += deltaSeconds;
}

void World::Update(double deltaSeconds)
{
	for (const UpdateFunction& updateFunction : updateFunctions)
	{
		updateFunction.execute(updateFunction.target, deltaSeconds);

#if _DEBUG
		++updateCallCount;
#endif
	}
}

void World::ProcessAll(double deltaSeconds)
{
	// World-local System 참여와 실행 순서를 확정한 이후 구현합니다.
	// 현재는 Process 단계의 경계와 delta time 전달 계약만 유지합니다.
	static_cast<void>(deltaSeconds);
}

void World::FinalUpdate(double deltaSeconds)
{
	for (const UpdateFunction& updateFunction : finalUpdateFunctions)
	{
		updateFunction.execute(updateFunction.target, deltaSeconds);

#if _DEBUG
	++finalUpdateCallCount;
#endif
	}
}
