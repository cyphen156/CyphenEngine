#include "pch.h"

#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/GameObject.h"
#include "Runtime/Public/World.h"
#include "Runtime/Public/WorldObject.h"

// 디버그용 Tick Call Count
#if _DEBUG
uint32 globalUpdateCallCount = 0;
uint32 globalFinalUpdateCallCount = 0;
#endif

bool GameRuntime::Admit(GameObject& gameObject)
{
	// Component 아래의 GameObject는 독립적인 Runtime 참여 루트가 될 수 없습니다.
	if (gameObject.IsRuntimeParticipationPathValid(*this) == false)
	{
		return false;
	}

	const Object* ancestor = gameObject.GetOuter();

	bool runtimeBoundaryClosed = false;

	// 대상에서 Outer 방향으로 Runtime 참여 구간의 연속성을 검사합니다.
	while (ancestor != nullptr)
	{
		const GameObject* outerGameObject = dynamic_cast<const GameObject*>(ancestor);

		if (outerGameObject == nullptr)
		{
			ancestor = ancestor->GetOuter();
			continue;
		}

		if (outerGameObject->runtime == nullptr)
		{
			runtimeBoundaryClosed = true;
			ancestor = ancestor->GetOuter();
			continue;
		}

		if (runtimeBoundaryClosed || outerGameObject->runtime != this)
		{
			return false;
		}

		ancestor = ancestor->GetOuter();
	}

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(&gameObject);

	// 실제 등록 전에 Subtree 전체의
	// Runtime 충돌을 검사합니다.
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

	constexpr UpdateParticipation updateParticipations[] =
	{
		UpdateParticipation::GlobalUpdate,
		UpdateParticipation::Update,
		UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalFinalUpdate
	};

	const uint32 admittedObjectBegin = static_cast<uint32>(gameObjects.size());

	bool admissionSucceeded = true;

	pendingObjects.push_back(&gameObject);

	// 부모 우선 DFS 순서로 Runtime 평면 장부와
	// 실행 그룹에 등록합니다.
	while (pendingObjects.empty() == false && admissionSucceeded)
	{
		Object* object = pendingObjects.back();
		pendingObjects.pop_back();

		GameObject* childGameObject = dynamic_cast<GameObject*>(object);

		if (childGameObject != nullptr && childGameObject->runtime == nullptr)
		{
			gameObjects.push_back(childGameObject);

			childGameObject->runtime = this;

			std::vector<UpdateFunction> updateFunctionList;

			for (UpdateParticipation participation : updateParticipations)
			{
				if (childGameObject->GetUpdateFunctions(participation, updateFunctionList) == false)
				{
					admissionSucceeded = false;
					break;
				}

				if (RegisterUpdateFunction(*childGameObject, participation, updateFunctionList) == false)
				{
					admissionSucceeded = false;
					break;
				}
			}
		}

		if (admissionSucceeded == false)
		{
			break;
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

	if (admissionSucceeded)
	{
		return true;
	}

	// 이번 Admit 호출이 추가한 gameObjects 접미부만 자식 우선 역순으로 롤백합니다.
	//
	// UpdateFunction 목록은 구조가 변경되지 않았으므로 각 GameObject에서 다시 생성할 수 있습니다.
	while (gameObjects.size() > admittedObjectBegin)
	{
		GameObject* admittedGameObject = gameObjects.back();

		std::vector<UpdateFunction> updateFunctionList;

		for (UpdateParticipation participation : updateParticipations)
		{
			if (admittedGameObject->GetUpdateFunctions(participation, updateFunctionList) == false)
			{
#if _DEBUG
				PRINT_DEBUG_OUTPUT("[GameRuntime] Admit 롤백 중 UpdateFunction 목록을 만들지 못했습니다.\n");
#endif
				continue;
			}

			if (UnregisterUpdateFunction(*admittedGameObject, participation, updateFunctionList) == false)
			{
#if _DEBUG
				PRINT_DEBUG_OUTPUT("[GameRuntime] Admit 롤백 중 UpdateFunction을 해제하지 못했습니다.\n");
#endif
			}
		}

		admittedGameObject->runtime = nullptr;
		gameObjects.pop_back();
	}

	return false;
}

bool GameRuntime::Leave(GameObject& gameObject)
{
	if (isInitialized == false || gameObject.runtime != this)
	{
		return false;
	}

	constexpr UpdateParticipation updateParticipations[] =
	{
		UpdateParticipation::GlobalUpdate,
		UpdateParticipation::Update,
		UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalFinalUpdate
	};

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(&gameObject);

	std::vector<GameObject*> leavingGameObjects;
	std::vector<WorldObject*> leavingWorldObjects;

	// 실제 소속을 변경하기 전에
	// Subtree 전체의 Runtime 장부를 검사합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();
		pendingObjects.pop_back();

		GameObject* childGameObject = dynamic_cast<GameObject*>(object);

		if (childGameObject != nullptr)
		{
			if (childGameObject->runtime != this)
			{
				return false;
			}

			bool isRegistered = false;

			for (GameObject* registeredGameObject : gameObjects)
			{
				if (registeredGameObject == childGameObject)
				{
					isRegistered = true;
					break;
				}
			}

			if (isRegistered == false)
			{
				return false;
			}

			leavingGameObjects.push_back(childGameObject);
		}

		WorldObject* childWorldObject = dynamic_cast<WorldObject*>(object);

		if (childWorldObject != nullptr && childWorldObject->GetWorld() != nullptr)
		{
			if (&childWorldObject->GetWorld()->GetGameRuntime() != this)
			{
				return false;
			}

			leavingWorldObjects.push_back(childWorldObject);
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

	// Runtime을 떠나는 객체는
	// World에 남아 있을 수 없습니다.
	//
	// 중첩 WorldObject는 서로 다른 World에 참여할 수 있으므로 자식부터 각자의 World를 떠납니다.
	for (uint32 index = static_cast<uint32>(leavingWorldObjects.size()); index > 0; --index)
	{
		WorldObject* worldObject = leavingWorldObjects[index - 1];

		World* targetWorld = worldObject->GetWorld();

		if (targetWorld == nullptr)
		{
			continue;
		}

		if (targetWorld->Leave(*worldObject) == false)
		{
			// Runtime 소속은 아직 유지됩니다.
			// 앞서 성공한 World 탈퇴는 복구하지 않습니다.
			return false;
		}
	}

	std::vector<UpdateFunction> updateFunctionList;

	// World 탈퇴가 끝난 뒤 모든 실행 참여를 자식부터 해제합니다.
	for (uint32 index = static_cast<uint32>(leavingGameObjects.size()); index > 0; --index)
	{
		GameObject* childGameObject = leavingGameObjects[index - 1];

		for (UpdateParticipation participation : updateParticipations)
		{
			if (childGameObject->GetUpdateFunctions(participation, updateFunctionList) == false)
			{
				return false;
			}

			if (UnregisterUpdateFunction(*childGameObject, participation, updateFunctionList) == false)
			{
				// Runtime 소속은 아직 유지됩니다.
				// 이미 해제한 실행 참여는 복구하지 않습니다.
				return false;
			}
		}
	}

	// 실패 가능한 정리가 모두 끝난 뒤
	// Runtime 평면 장부 제거를 커밋합니다.
	for (uint32 index = static_cast<uint32>(leavingGameObjects.size()); index > 0; --index)
	{
		GameObject* childGameObject = leavingGameObjects[index - 1];

		std::vector<GameObject*>::iterator iterator;

		for (iterator = gameObjects.begin(); iterator != gameObjects.end(); ++iterator)
		{
			if (*iterator == childGameObject)
			{
				break;
			}
		}

		if (iterator == gameObjects.end())
		{
#if _DEBUG
			PRINT_DEBUG_OUTPUT("[GameRuntime] Leave 커밋 중 GameObject 장부가 변경되었습니다.\n");
#endif
			return false;
		}

		gameObjects.erase(iterator);
		childGameObject->runtime = nullptr;
	}

	return true;
}

bool GameRuntime::RegisterUpdateFunction(GameObject& gameObject, 
	UpdateParticipation participation, const std::vector<UpdateFunction>& updateFunctionList)
{
	if (isInitialized == false || gameObject.runtime != this)
	{
		return false;
	}

	const World* targetWorld = nullptr;

	const uint8 worldParticipation =
		static_cast<uint8>(UpdateParticipation::Update) |
		static_cast<uint8>(UpdateParticipation::FinalUpdate);

	// Update와 FinalUpdate는 World-local 실행 단계이므로
	// 전달받은 GameObject의 현재 계층에서 World anchor를 찾습니다.
	if ((static_cast<uint8>(participation) & worldParticipation) != 0)
	{
		Object* ancestor = &gameObject;

		while (ancestor != nullptr)
		{
			WorldObject* worldObject = dynamic_cast<WorldObject*>(ancestor);

			if (worldObject == nullptr)
			{
				ancestor = ancestor->GetOuter();
				continue;
			}

			targetWorld = worldObject->GetWorld();
			break;
		}

		// World anchor가 없거나 최초 WorldObject가 아직 World에
		// 합류하지 않았다면 World-local 실행에는 등록하지 않습니다.
		if (targetWorld == nullptr)
		{
			return true;
		}

		if (&targetWorld->GetGameRuntime() != this)
		{
			return false;
		}
	}

	const uint32 updateFunctionCount = static_cast<uint32>(updateFunctionList.size());

	for (uint32 registeredFunctionCount = 0; registeredFunctionCount < updateFunctionCount; ++registeredFunctionCount)
	{
		const UpdateFunction& updateFunction = updateFunctionList[registeredFunctionCount];

		bool registrationResult = false;

		switch (participation)
		{
		case UpdateParticipation::GlobalUpdate:
			registrationResult = updateManager.RegisterGlobalUpdate(updateFunction);
			break;

		case UpdateParticipation::Update:
			registrationResult = updateManager.RegisterUpdate(*targetWorld, updateFunction);
			break;

		case UpdateParticipation::FinalUpdate:
			registrationResult = updateManager.RegisterFinalUpdate(*targetWorld, updateFunction);
			break;

		case UpdateParticipation::GlobalFinalUpdate:
			registrationResult = updateManager.RegisterGlobalFinalUpdate(updateFunction);
			break;

		default:
			return false;
		}

		if (registrationResult)
		{
			continue;
		}

		// 이번 요청에서 앞서 등록한 실행 함수만 역순으로 제거합니다.
		//
		// UpdateManager는 중복 등록을 실패로 처리하므로
		// 성공한 앞부분은 모두 이번 호출에서 새로 등록한 대상입니다.
		while (registeredFunctionCount > 0)
		{
			--registeredFunctionCount;

			const UpdateFunction& rollbackFunction = updateFunctionList[registeredFunctionCount];

			switch (participation)
			{
			case UpdateParticipation::GlobalUpdate:
				updateManager.UnregisterGlobalUpdate(rollbackFunction);
				break;

			case UpdateParticipation::Update:
				updateManager.UnregisterUpdate(*targetWorld, rollbackFunction);
				break;

			case UpdateParticipation::FinalUpdate:
				updateManager.UnregisterFinalUpdate(*targetWorld, rollbackFunction);
				break;

			case UpdateParticipation::GlobalFinalUpdate:
				updateManager.UnregisterGlobalFinalUpdate(rollbackFunction);
				break;

			default:
				break;
			}
		}

		return false;
	}

	return true;
}

bool GameRuntime::UnregisterUpdateFunction(GameObject& gameObject, 
	UpdateParticipation participation, const std::vector<UpdateFunction>& updateFunctionList)
{
	if (isInitialized == false || gameObject.runtime != this)
	{
		return false;
	}

	const World* targetWorld = nullptr;

	const uint8 worldParticipation =
		static_cast<uint8>(UpdateParticipation::Update) |
		static_cast<uint8>(UpdateParticipation::FinalUpdate);

	// Update와 FinalUpdate는 등록 당시 사용한 World 문맥이 유지되는 동안 해제해야 합니다.
	if ((static_cast<uint8>(participation) & worldParticipation) != 0)
	{
		Object* ancestor = &gameObject;

		while (ancestor != nullptr)
		{
			WorldObject* worldObject = dynamic_cast<WorldObject*>(ancestor);

			if (worldObject == nullptr)
			{
				ancestor = ancestor->GetOuter();
				continue;
			}

			targetWorld = worldObject->GetWorld();
			break;
		}

		// 해당 World 문맥에 등록된 실행 함수가 없다면
		// 별도의 해제 작업 없이 성공합니다.
		if (targetWorld == nullptr)
		{
			return true;
		}

		if (&targetWorld->GetGameRuntime() != this)
		{
			return false;
		}
	}

	const uint32 updateFunctionCount = static_cast<uint32>(updateFunctionList.size());

	
	for (uint32 unregisteredFunctionCount = 0; unregisteredFunctionCount < updateFunctionCount; ++unregisteredFunctionCount)
	{
		const UpdateFunction& updateFunction = updateFunctionList[unregisteredFunctionCount];

		bool unregistrationResult = false;

		switch (participation)
		{
		case UpdateParticipation::GlobalUpdate:
			unregistrationResult = updateManager.UnregisterGlobalUpdate(updateFunction);
			break;

		case UpdateParticipation::Update:
			unregistrationResult = updateManager.UnregisterUpdate(*targetWorld, updateFunction);
			break;

		case UpdateParticipation::FinalUpdate:
			unregistrationResult = updateManager.UnregisterFinalUpdate(*targetWorld, updateFunction);
			break;

		case UpdateParticipation::GlobalFinalUpdate:
			unregistrationResult = updateManager.UnregisterGlobalFinalUpdate(updateFunction);
			break;

		default:
			return false;
		}

		if (unregistrationResult)
		{
			continue;
		}

		// 앞서 해제한 실행 함수를 역순으로 다시 등록하여
		// 호출 이전의 실행 참여 상태를 복구합니다.
		while (unregisteredFunctionCount > 0)
		{
			--unregisteredFunctionCount;

			const UpdateFunction& rollbackFunction = updateFunctionList[unregisteredFunctionCount];

			switch (participation)
			{
			case UpdateParticipation::GlobalUpdate:
				updateManager.RegisterGlobalUpdate(rollbackFunction);
				break;

			case UpdateParticipation::Update:
				updateManager.RegisterUpdate(*targetWorld, rollbackFunction);
				break;

			case UpdateParticipation::FinalUpdate:
				updateManager.RegisterFinalUpdate(*targetWorld, rollbackFunction);
				break;

			case UpdateParticipation::GlobalFinalUpdate:
				updateManager.RegisterGlobalFinalUpdate(rollbackFunction);
				break;

			default:
				break;
			}
		}

		return false;
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

	updateManager.Clear();

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
#if _DEBUG
	globalUpdateCallCount = 0;
#endif

	const std::vector<UpdateFunctionGroup>& functionGroups = updateManager.GetGlobalUpdateGroups();

	for (const UpdateFunctionGroup& functionGroup : functionGroups)
	{
		if (functionGroup.execute == nullptr)
		{
			continue;
		}

		for (Object* target : functionGroup.targets)
		{
			if (target == nullptr)
			{
				continue;
			}

			functionGroup.execute(target, deltaSeconds);

#if _DEBUG
			++globalUpdateCallCount;
#endif
		}
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
#if _DEBUG
	globalFinalUpdateCallCount = 0;
#endif

	const std::vector<UpdateFunctionGroup>& functionGroups = updateManager.GetGlobalFinalUpdateGroups();

	for (const UpdateFunctionGroup& functionGroup : functionGroups)
	{
		if (functionGroup.execute == nullptr)
		{
			continue;
		}

		for (Object* target : functionGroup.targets)
		{
			if (target == nullptr)
			{
				continue;
			}

			functionGroup.execute(target, deltaSeconds);

#if _DEBUG
			++globalFinalUpdateCallCount;
#endif
		}
	}
}

World* GameRuntime::CreateWorld()
{
	if (isInitialized == false)
	{
		return nullptr;
	}

	World* world = new World(*this, updateManager);
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

uint32 GameRuntime::GetWorldCount() const
{
	return static_cast<uint32>(worlds.size());
}

World* GameRuntime::GetWorld(uint32 worldIndex)
{
	if (worldIndex >= worlds.size())
	{
		return nullptr;
	}

	return worlds[worldIndex];
}

bool GameRuntime::Destroy(GameObject& gameObject)
{
	if (gameObject.runtime != this)
	{
		return false;
	}

	// 현재 단계에서는 검증 가능한 살아 있는 GameObject만 처리합니다.
	//
	// Runtime 및 World 계약을 정상 순서로 먼저 제거한 뒤
	// ObjectManager의 지연 파괴 경로를 호출합니다.
	//
	// Leave조차 수행할 수 없는 손상 객체의 강제수집은
	// 향후 ObjectManager 또는 GC의 별도 우회 경로로 분리합니다.
	if (Leave(gameObject) == false)
	{
		return false;
	}

	return gameObject.Destroy();
}
