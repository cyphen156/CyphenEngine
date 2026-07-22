#include "pch.h"

#include "Runtime/Public/GameObject.h"
#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/World.h"

bool GameRuntime::Admit(GameObject& gameObject)
{
	if (isInitialized == false || gameObject.runtime != nullptr || gameObject.GetOuter() != nullptr)
	{
		return false;
	}

	gameObjects.push_back(&gameObject);
	gameObject.runtime = this;

	return true;
}

bool GameRuntime::IsInitialized() const
{
	return isInitialized;
}

GameRuntime::GameRuntime() = default;

bool GameRuntime::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	isInitialized = true;

	return true;
}

void GameRuntime::Shutdown()
{
	if (isInitialized == false)
	{
		return;
	}

	while (gameObjects.empty() == false)
	{
		GameObject* gameObject = gameObjects.back();

		if (gameObject == nullptr || gameObject->Destroy() == false)
		{
			break;
		}
	}

	while (worlds.empty() == false)
	{
		World* world = worlds.back();

		if (world == nullptr || DestroyWorld(*world) == false)
		{
			break;
		}
	}

	isInitialized = false;
}

void GameRuntime::Tick(double deltaSeconds)
{
	if (isInitialized == false || deltaSeconds < 0.0)
	{
		return;
	}

	GlobalUpdate(deltaSeconds);
	ProcessBeforeWorldTicks(deltaSeconds);

	for (World* world : worlds)
	{
		if (world != nullptr)
		{
			world->Tick(deltaSeconds);
		}
	}

	ProcessAfterWorldTicks(deltaSeconds);
	GlobalFinalUpdate(deltaSeconds);
}

void GameRuntime::GlobalUpdate(double deltaSeconds)
{
	// Runtime 전체 범위의 OOP 선행 실행 계약을 확정한 이후 구현합니다.
	// 현재는 GlobalUpdate 단계의 경계와 delta time 전달만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::ProcessBeforeWorldTicks(double deltaSeconds)
{
	// World 실행 전에 필요한 Runtime-global System 처리 계약을
	// 확정한 이후 구현합니다.
	// 현재는 모든 World Tick 이전의 Process 단계 경계만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::ProcessAfterWorldTicks(double deltaSeconds)
{
	// 모든 World 실행 결과를 사용하는 Runtime-global System 처리 계약을
	// 확정한 이후 구현합니다.
	// 현재는 모든 World Tick 이후의 Process 단계 경계만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::GlobalFinalUpdate(double deltaSeconds)
{
	// Runtime 전체 범위의 OOP 후행 실행 계약을 확정한 이후 구현합니다.
	// 현재는 GlobalFinalUpdate 단계의 경계와 delta time 전달만 보장합니다.
	static_cast<void>(deltaSeconds);
}

World* GameRuntime::CreateWorld()
{
	if (isInitialized == false)
	{
		return nullptr;
	}

	World* world = new World(*this);
	worlds.push_back(world);

	return world;
}

bool GameRuntime::DestroyWorld(World& world)
{
	if (&world.GetGameRuntime() != this)
	{
		return false;
	}

	std::vector<World*>::iterator iterator;
	for (iterator = worlds.begin(); iterator != worlds.end(); ++iterator)
	{
		if (*iterator == &world)
		{
			break;
		}
	}

	if (iterator == worlds.end())
	{
		return false;
	}

	worlds.erase(iterator);
	delete& world;

	return true;
}

bool GameRuntime::Destroy(GameObject& gameObject)
{
	if (gameObject.runtime != this)
	{
		return false;
	}

	std::vector<GameObject*>::iterator iterator;
	for (iterator = gameObjects.begin(); iterator != gameObjects.end(); ++iterator)
	{
		if (*iterator == &gameObject)
		{
			break;
		}
	}

	if (iterator == gameObjects.end())
	{
		return false;
	}

	gameObjects.erase(iterator);
	gameObject.runtime = nullptr;

	return true;
}
