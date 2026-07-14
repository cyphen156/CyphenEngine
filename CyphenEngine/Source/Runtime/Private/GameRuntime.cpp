#include "pch.h"

#include "Runtime/Public/GameRuntime.h"

bool GameRuntime::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	world.Reset();
	isInitialized = true;
	return true;
}

void GameRuntime::Tick(double deltaSeconds)
{
	if (isInitialized == false || deltaSeconds < 0.0)
	{
		return;
	}

	world.Tick(deltaSeconds);
}

void GameRuntime::Shutdown()
{
	if (isInitialized == false)
	{
		return;
	}

	world.Reset();
	isInitialized = false;
}

const World& GameRuntime::GetWorld() const
{
	return world;
}

bool GameRuntime::IsInitialized() const
{
	return isInitialized;
}
