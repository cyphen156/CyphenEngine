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
	// 실행 참여 대상과 Scheduler 계약이 확정된 이후 구현합니다.
	// World는 현재 선행 실행 단계의 경계만 보장합니다.
}

void World::ProcessAll(double deltaSeconds)
{
	// World-local System의 참여와 실행 순서가 확정된 이후 구현합니다.
	// 현재는 Process 단계의 경계와 delta time 전달 계약만 유지합니다.
	static_cast<void>(deltaSeconds);
}

void World::FinalUpdate()
{
	// 실행 참여 대상과 Scheduler 계약이 확정된 이후 구현합니다.
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
