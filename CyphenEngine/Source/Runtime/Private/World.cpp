#include "pch.h"

#include "Runtime/Public/World.h"
#include "Runtime/Public/GameRuntime.h"

// 디버그용 Tick Call Count
#if _DEBUG
uint32 updateCallCount = 0;
uint32 finalUpdateCallCount = 0;
#endif

// WorldObject를 현재 World에 합류시킵니다.
// 하위 WorldObject를 재귀적으로 합류시키지 않습니다.
bool World::Join(WorldObject& worldObject, const Transform& initialTransform)
{
	// 이미 어딘가에 소속되어있는 경우
	if (worldObject.world != nullptr)
	{
		return false;
	}

	const GameRuntime* gameRuntime = worldObject.GetGameRuntime();
	// 다른 Runtime에 참여 중인 WorldObject는 현재 Runtime이 소유한 World에 합류할 수 없습니다.
	if (gameRuntime != nullptr && gameRuntime != &owningRuntime)
	{
		return false;
	}

	// World 참여는 Runtime 참여를 전제로 합니다.
	// 아직 Runtime에 참여하지 않았다면 현재 Runtime에 먼저 참여시킵니다.
	if (gameRuntime == nullptr && owningRuntime.Admit(worldObject) == false)
	{
		return false;
	}

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(
		&worldObject);

	std::vector<UpdateFunction>
		updateFunctionList;

	std::vector<UpdateFunction>
		finalUpdateFunctionList;

	std::vector<UpdateFunction>
		objectUpdateFunctionList;

	// 현재 WorldObject가 World anchor인 실행 범위의
	// Update와 FinalUpdate 함수를 먼저 수집합니다.
	//
	// 일반 Object와 Component는 하위 GameObject를 찾기 위한
	// 구조적 통로로 사용합니다.
	//
	// 다른 WorldObject를 만나면 그 노드부터 별도의 World anchor이므로
	// 해당 노드와 전체 하위 가지를 현재 목록에서 제외합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();

		pendingObjects.pop_back();

		WorldObject* childWorldObject = dynamic_cast<WorldObject*>(object);

		if (object != &worldObject && childWorldObject != nullptr)
		{
			continue;
		}

		GameObject* gameObject = dynamic_cast<GameObject*>(object);

		if (gameObject != nullptr)
		{
			if (gameObject->GetUpdateFunctions(UpdateParticipation::Update, objectUpdateFunctionList) == false)
			{
				return false;
			}

			updateFunctionList.insert(updateFunctionList.end(), objectUpdateFunctionList.begin(), objectUpdateFunctionList.end());

			if (gameObject->GetUpdateFunctions(UpdateParticipation::FinalUpdate, objectUpdateFunctionList) == false)
			{
				return false;
			}

			finalUpdateFunctionList.insert(finalUpdateFunctionList.end(), objectUpdateFunctionList.begin(), objectUpdateFunctionList.end());
		}

		const uint32 subObjectCount = object->GetSubObjectCount();

		// 스택을 사용하면서 기존 SubObject 순서를 유지하도록
		// 자식은 역순으로 삽입합니다.
		for (uint32 index = 0; index < subObjectCount; ++index)
		{
			Object* subObject = object->GetSubObject(subObjectCount - index - 1);

			if (subObject != nullptr)
			{
				pendingObjects.push_back(subObject);
			}
		}
	}

	const StorageSlot<Transform> transformSlot = transforms.Insert(worldObject.GetHandle(), initialTransform);

	if (transformSlot.IsSet() == false)
	{
		return false;
	}

	worldObjects.push_back(&worldObject);

	worldObject.world = this;

	// world가 설정된 이후 등록해야 GameRuntime이
	// 현재 World-local 실행 목적지를 확인할 수 있습니다.
	if (owningRuntime.RegisterUpdateFunction(worldObject, UpdateParticipation::Update, updateFunctionList) == false)
	{
		worldObjects.pop_back();
		worldObject.world = nullptr;

		transforms.Remove(worldObject.GetHandle());

		return false;
	}

	if (owningRuntime.RegisterUpdateFunction(worldObject, UpdateParticipation::FinalUpdate, finalUpdateFunctionList) == false)
	{
		owningRuntime.UnregisterUpdateFunction(worldObject, UpdateParticipation::Update, updateFunctionList);

		worldObjects.pop_back();
		worldObject.world = nullptr;

		transforms.Remove(worldObject.GetHandle());

		return false;
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

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(&worldObject);

	std::vector<UpdateFunction>updateFunctionList;

	std::vector<UpdateFunction>finalUpdateFunctionList;

	std::vector<UpdateFunction>objectUpdateFunctionList;

	// Join과 동일한 World anchor 범위의
	// Update와 FinalUpdate 실행 함수를 수집합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();

		pendingObjects.pop_back();

		WorldObject* childWorldObject = dynamic_cast<WorldObject*>(object);

		if (object != &worldObject && childWorldObject != nullptr)
		{
			continue;
		}

		GameObject* gameObject = dynamic_cast<GameObject*>(object);

		if (gameObject != nullptr)
		{
			if (gameObject->GetUpdateFunctions(UpdateParticipation::Update, objectUpdateFunctionList) == false)
			{
				return false;
			}

			updateFunctionList.insert(updateFunctionList.end(), objectUpdateFunctionList.begin(), objectUpdateFunctionList.end());

			if (gameObject->GetUpdateFunctions(UpdateParticipation::FinalUpdate, objectUpdateFunctionList) == false)
			{
				return false;
			}

			finalUpdateFunctionList.insert(finalUpdateFunctionList.end(), objectUpdateFunctionList.begin(), objectUpdateFunctionList.end());
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

	// World와 Outer 문맥이 유지되는 동안
	// World-local 실행 참여를 먼저 해제합니다.
	if (owningRuntime.UnregisterUpdateFunction(worldObject, UpdateParticipation::Update, updateFunctionList) == false)
	{
		return false;
	}

	if (owningRuntime.UnregisterUpdateFunction(worldObject, UpdateParticipation::FinalUpdate, finalUpdateFunctionList) == false)
	{
		owningRuntime.RegisterUpdateFunction(worldObject, UpdateParticipation::Update, updateFunctionList);

		return false;
	}

	if (transforms.Remove(worldObject.GetHandle()) == false)
	{
		owningRuntime.RegisterUpdateFunction(worldObject, UpdateParticipation::Update, updateFunctionList);

		owningRuntime.RegisterUpdateFunction(worldObject, UpdateParticipation::FinalUpdate, finalUpdateFunctionList);

		return false;
	}

	worldObjects.erase(iterator);
	worldObject.world = nullptr;

	// Leave는 World 참여와 U/FU만 제거합니다.
	// Runtime 참여와 GU/GFU는 그대로 유지합니다.
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

World::World(GameRuntime& owningRuntime, UpdateManager& updateManager)
	: owningRuntime(owningRuntime),
	updateManager(updateManager)
{
	updateFunctionGroups = updateManager.CreateWorldFunctionGroups(*this);
}

World::~World()
{
	Reset();
	updateManager.DestroyWorldFunctionGroups(*this);
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
#if _DEBUG
	updateCallCount = 0;
#endif

	if (updateFunctionGroups == nullptr)
	{
		return;
	}

	for (const UpdateFunctionGroup& functionGroup : updateFunctionGroups->updateGroups)
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
			++updateCallCount;
#endif
		}
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
#if _DEBUG
	finalUpdateCallCount = 0;
#endif

	if (updateFunctionGroups == nullptr)
	{
		return;
	}

	for (const UpdateFunctionGroup& functionGroup : updateFunctionGroups->finalUpdateGroups)
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
			++finalUpdateCallCount;
#endif
		}
	}
}
