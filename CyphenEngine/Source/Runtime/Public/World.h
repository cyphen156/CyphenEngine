#pragma once

#include <utility>
#include <vector>

#include "Runtime/Public/GameRuntime.h"
#include "Core/Public/CPrimitiveTypes.h"
#include "Runtime/Public/Chunk.h"
#include "Runtime/Public/ComponentDataStorage.h"
#include "Runtime/Public/UpdateManager.h"
#include "Runtime/Public/Object.h"
#include "Runtime/Public/WorldObject.h"
#include "Core/Public/Math/Transform.h"

// ============================================================================
// World
// ----------------------------------------------------------------------------
// Object와 System이 살아가는 독립된 논리 시뮬레이션 공간입니다.
//
// World는 GameRuntime만 생성하고 파괴합니다.
// 생성 시점부터 하나의 GameRuntime에 반드시 속하며 무소속 상태를 허용하지 않습니다.
// owningRuntime 참조는 World의 전체 수명 동안 변경되지 않습니다.
//
// World는 WorldObject의 메모리와 수명을 소유하지 않습니다.
// World-local ComponentData 정본과 소속 WorldObject의 비소유 참조를 유지합니다.
// World-local Function Group은 소유 Runtime의 UpdateManager가 관리합니다.
// World는 자신에게 공급된 Function Group의 읽기 전용 포인터만 보유하고 실행합니다.
//
// Spawn:
//   - WorldObject 계층을 새로 생성합니다.
//   - 완성된 계층의 모든 GameObject를 소유 Runtime에 Admit합니다.
//   - 완성된 계층에 포함된 모든 WorldObject를 현재 World에 Join합니다.
//   - 루트 WorldObject에는 전달받은 initialTransform을 적용합니다.
//   - 하위 WorldObject에는 별도 원형 데이터가 없으므로 기본 Transform을 적용합니다.
//   - 각 WorldObject의 Join이 자신을 anchor로 하는 실행 범위의
//     World-local Update / FinalUpdate 참여를 등록합니다.
//   - GlobalUpdate / GlobalFinalUpdate는 Admit에서 이미 등록되었으며
//     Join은 Runtime-global 등록을 변경하지 않습니다.
//
// Join:
//   - 생성된 WorldObject 하나를 현재 World의 논리 공간에 합류시킵니다.
//   - 다른 Runtime에 Admit된 WorldObject는 허용하지 않으며,
//     아직 Runtime에 참여하지 않았다면 소유 Runtime에 먼저 Admit합니다.
//   - WorldObject의 Transform 정본을 생성합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 연결합니다.
//   - 자신을 anchor로 하는 실행 범위의 Update / FinalUpdate 참여를 등록합니다.
//     하위 WorldObject 가지는 별도 anchor이므로 범위에서 제외합니다.
//   - 하위 WorldObject를 재귀적으로 합류시키지 않습니다.
//
// Leave:
//   - 자신을 anchor로 하는 실행 범위의 Update / FinalUpdate 등록을 해제합니다.
//   - WorldObject의 Transform 정본을 제거합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 해제합니다.
//   - Runtime 참여와 Runtime-global 등록은 유지합니다.
//   - WorldObject의 메모리와 Object 수명은 변경하지 않습니다.
//
// TryGetTransform:
//   - ObjectHandle을 통해 현재 World가 소유한 Transform 정본을 조회합니다.
//   - World에 배치된 객체가 자신의 World-local Transform을 찾는 public 조회 경계입니다.
//
// Tick:
//   - GameRuntime이 simulation을 전진시킬 때 호출합니다.
//   - 한 번의 호출은 한 번의 World simulation step을 의미합니다.
//   - Update -> ProcessAll -> FinalUpdate 순서로 실행합니다.
//   - Update와 FinalUpdate는 등록된 OOP 실행 함수를 순회합니다.
//   - ProcessAll은 World-local System 실행 단계의 경계입니다.
//   - 플랫폼 시간을 직접 조회하지 않고 GameRuntime이 전달한 delta time만 사용합니다.
//
// 책임:
//   - 현재 World-local simulation 상태 소유
//   - 공유 ComponentData 정본 소유
//   - 소속 WorldObject의 비소유 참조와 World-local 표현 연결
//   - UpdateManager를 통한 World-local Function Group 등록과 해제
//   - 공급받은 World-local Function Group의 직접 실행
//   - Runtime이 요청한 한 번의 simulation step 수행
//   - Update -> ProcessAll -> FinalUpdate 실행 단계 경계 보장
//   - Snapshot 대상이 되는 논리 상태 유지
//
// 비책임:
//   - 자체 Run loop 소유
//   - delta time 산출과 Tick 호출 정책
//   - Render Frame 생성과 Renderer 연결
//   - WorldObject 메모리와 Object 수명 소유
//   - ObjectHandle 발급과 Object 메모리 소유
//   - Component 부착과 System 참여 정책
// ============================================================================

class World final
{
public:
	template<typename WorldObjectType, typename... ArgumentTypes>
	WorldObjectType* Spawn(const Transform& initialTransform, ArgumentTypes&&... arguments);

	bool Join(WorldObject& worldObject, const Transform& initialTransform);
	bool Leave(WorldObject& worldObject);

	uint64 GetSimulationTick() const;
	double GetSimulationTime() const;
	const GameRuntime& GetGameRuntime() const;

	bool TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const;

private:
	friend class GameRuntime;
	friend class WorldObject;

	World(const World&) = delete;
	World& operator=(const World&) = delete;
	World(World&&) = delete;
	World& operator=(World&&) = delete;

	World(GameRuntime& owningRuntime, UpdateManager& updateManager);
	~World();

	void Reset();
	void Tick(double deltaSeconds);

	// World Loop Phases
	void Update(double deltaSeconds);
	void ProcessAll(double deltaSeconds);
	void FinalUpdate(double deltaSeconds);

	uint64 simulationTick = 0;
	double simulationTime = 0.0;

	GameRuntime& owningRuntime;
	UpdateManager& updateManager;
	const WorldUpdateFunctionGroups* updateFunctionGroups = nullptr;

	Chunk worldOrigin = {};

	std::vector<WorldObject*> worldObjects;
	ComponentDataStorage<Transform, ObjectHandle> transforms;
};

template<typename WorldObjectType, typename... ArgumentTypes>
WorldObjectType* World::Spawn(const Transform& initialTransform, ArgumentTypes&&... arguments)
{
	WorldObjectType* spawnedObject = Object::NewObject<WorldObjectType>(std::forward<ArgumentTypes>(arguments)...);

	if (spawnedObject == nullptr)
	{
		return nullptr;
	}

	// World::Spawn의 루트는 반드시 WorldObject 파생 타입이어야 합니다.
	// 이 제약은 Spawn 루트에만 적용되며,
	// SubObject에는 Object, Component, GameObject와 WorldObject를 모두 사용할 수 있습니다.
	WorldObject* worldObject = spawnedObject;

	std::vector<Object*> pendingObjects;
	pendingObjects.push_back(worldObject);

	// 완성된 Aggregate에 포함된 모든 WorldObject를
	// 현재 World에 부모 우선 DFS 순서로 합류시킵니다.
	//
	// 첫 번째 루트 Join은 Runtime에 참여하지 않은 전체 GameObject 계층을
	// 현재 World의 소유 Runtime에 Admit합니다.
	//
	// 이후 발견되는 하위 WorldObject는 이미 같은 Runtime에 참여하므로
	// 각 WorldObject의 World 소속과 Transform만 구성합니다.
	while (pendingObjects.empty() == false)
	{
		Object* object = pendingObjects.back();
		pendingObjects.pop_back();

		WorldObject* childWorldObject = dynamic_cast<WorldObject*>(object);

		if (childWorldObject != nullptr)
		{
			// 루트에는 호출자가 전달한 Transform을 적용합니다.
			// 하위 WorldObject는 별도 원형 Transform 공급 경로가 없으므로
			// 현재는 기본 Transform을 적용합니다.
			const Transform objectTransform =
				object == worldObject ?
				initialTransform :
				Transform::Identity();

			if (Join(*childWorldObject, objectTransform) == false)
			{
				// 루트 Join이 Runtime 참여까지만 성공했거나,
				// 일부 WorldObject의 Join만 성공한 경우에도
				// 생성된 전체 계층을 파괴하여 Spawn을 원복합니다.
				worldObject->Destroy();

				return nullptr;
			}
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

	return spawnedObject;
}
