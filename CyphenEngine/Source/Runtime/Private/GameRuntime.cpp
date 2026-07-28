#include "pch.h"

#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/GameObject.h"
#include "Runtime/Public/World.h"

// 디버그용 Tick Call Count
#if _DEBUG
uint32 globalUpdateCallCount = 0;
uint32 globalFinalUpdateCallCount = 0;
#endif

bool GameRuntime::Admit(GameObject& gameObject)
{
	if (isInitialized == false || gameObject.runtime != nullptr)
	{
		return false;
	}

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(&gameObject);

	// 실제 등록 전에 전체 계층을 먼저 확인합니다.
	// 순수 Object와 Component는 Runtime 소속 대상이 아니므로
	// 구체 타입을 판별하지 않고 하위 SubObject 탐색만 계속합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();
		pendingObjects.pop_back();

		GameObject* childGameObject = dynamic_cast<GameObject*>(object);

		if (childGameObject != nullptr &&
			childGameObject->runtime != nullptr &&
			childGameObject->runtime != this)
		{
			return false;
		}

		const uint32 subObjectCount = object->GetSubObjectCount();

		for (uint32 index = 0; index < subObjectCount; ++index)
		{
			Object* subObject = object->GetSubObject(subObjectCount - index - 1);

			if (subObject != nullptr)
			{
				pendingObjects.push_back(subObject);
			}
		}
	}

	pendingObjects.push_back(&gameObject);

	// 검사가 끝난 뒤 계층에 포함된 모든 GameObject를
	// Runtime의 평면 gameObjects 장부에 등록합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();
		pendingObjects.pop_back();

		GameObject* childGameObject =
			dynamic_cast<GameObject*>(object);

		if (childGameObject != nullptr && childGameObject->runtime == nullptr)
		{
			gameObjects.push_back(childGameObject);
			childGameObject->runtime = this;
		}

		const uint32 subObjectCount = object->GetSubObjectCount();

		for (uint32 index = 0; index < subObjectCount; ++index)
		{
			Object* subObject = object->GetSubObject( subObjectCount - index - 1);

			if (subObject != nullptr)
			{
				pendingObjects.push_back(subObject);
			}
		}
	}
	if (gameObject.HasUpdateParticipation(UpdateParticipation::GlobalUpdate))
	{
		globalUpdateFunctions.push_back(
			{
				&gameObject,
				ExecuteUpdateFunction<GameObject, &GameObject::GlobalUpdate>
			});
	}

	if (gameObject.HasUpdateParticipation(UpdateParticipation::GlobalFinalUpdate))
	{
		globalFinalUpdateFunctions.push_back(
			{
				&gameObject,
				ExecuteUpdateFunction<GameObject, &GameObject::GlobalFinalUpdate>
			});
	}
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

	globalUpdateFunctions.clear();
	globalFinalUpdateFunctions.clear();

	isInitialized = false;
}

void GameRuntime::Tick(double deltaSeconds)
{
	if (isInitialized == false || deltaSeconds < 0.0)
	{
		return;
	}

// 프레임 틱 콜 카운트 초기화
#if _DEBUG
	globalUpdateCallCount = 0;
	globalFinalUpdateCallCount = 0;
#endif

	GlobalUpdate(deltaSeconds);
	ProcessBeforeWorldTicks(deltaSeconds);

	for (World* world : worlds)
	{
		if (world == nullptr)
		{
			continue;
		}

		world->Tick(deltaSeconds);
	}

	ProcessAfterWorldTicks(deltaSeconds);
	GlobalFinalUpdate(deltaSeconds);
}

void GameRuntime::GlobalUpdate(double deltaSeconds)
{
	for (const UpdateFunction& updateFunction : globalUpdateFunctions)
	{
		updateFunction.execute(updateFunction.target, deltaSeconds);

#if _DEBUG
		globalUpdateCallCount++;
#endif
	}
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
	for (const UpdateFunction& updateFunction : globalFinalUpdateFunctions)
	{
		updateFunction.execute(updateFunction.target, deltaSeconds);

#if _DEBUG
		globalFinalUpdateCallCount++;
#endif
	}
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

	const uint32 globalUpdateCount = static_cast<uint32>(globalUpdateFunctions.size());

	for (uint32 index = 0; index < globalUpdateCount; ++index)
	{
		if (globalUpdateFunctions[index].target != &gameObject)
		{
			continue;
		}

		globalUpdateFunctions[index] = globalUpdateFunctions.back();
		globalUpdateFunctions.pop_back();

		break;
	}

	const uint32 globalFinalUpdateCount = static_cast<uint32>(globalFinalUpdateFunctions.size());

	for (uint32 index = 0; index < globalFinalUpdateCount; ++index)
	{
		if (globalFinalUpdateFunctions[index].target != &gameObject)
		{
			continue;
		}

		globalFinalUpdateFunctions[index] = globalFinalUpdateFunctions.back();
		globalFinalUpdateFunctions.pop_back();

		break;
	}

	gameObjects.erase(iterator);
	gameObject.runtime = nullptr;

	return true;
}
