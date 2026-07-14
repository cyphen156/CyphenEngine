#include "pch.h"

#include "Runtime/Public/World.h"

void World::Reset()
{
	simulationTick = 0;
	simulationTime = 0.0;
	worldOrigin = {};
	transforms.Clear();
}

void World::Tick(double deltaSeconds)
{
	if (deltaSeconds < 0.0)
	{
		return;
	}

	++simulationTick;
	simulationTime += deltaSeconds;
}

uint64 World::GetSimulationTick() const
{
	return simulationTick;
}

double World::GetSimulationTime() const
{
	return simulationTime;
}
