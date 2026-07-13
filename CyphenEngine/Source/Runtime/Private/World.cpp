#include "pch.h"

#include "Runtime/Public/World.h"

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
