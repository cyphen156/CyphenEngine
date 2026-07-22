#pragma once

#include <utility>
#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/Math/Transform.h"
#include "Runtime/Public/Chunk.h"
#include "Runtime/Public/ComponentDataStorage.h"
#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/Object.h"

class WorldObject;

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
//
// Spawn:
//   - WorldObject를 새로 생성하고 소유 Runtime에 Admit한 뒤 현재 World에 Join합니다.
//   - 대상 World와 최초 Transform을 명시하는 생성 API입니다.
//
// Join:
//   - 생성된 WorldObject를 현재 World의 논리 공간에 합류시킵니다.
//   - 현재 World와 같은 GameRuntime에 Admit된 WorldObject만 허용합니다.
//   - WorldObject의 Transform 정본을 생성합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 연결합니다.
//   - Update 또는 System 실행 참여를 자동으로 결정하지 않습니다.
//
// Leave:
//   - WorldObject의 Transform 정본을 제거합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 해제합니다.
//   - WorldObject의 메모리와 Object 수명은 변경하지 않습니다.
//
// TryGetTransform:
//   - ObjectHandle을 통해 현재 World가 소유한 Transform 정본을 조회합니다.
//   - World에 배치된 객체가 자신의 World-local Transform을 찾는 public 조회 경계입니다.
//
// Tick:
//   - GameRuntime이 simulation을 전진시킬 때 호출합니다.
//   - 한 번의 호출은 한 번의 World simulation step을 의미합니다.
//   - PreUpdate -> ProcessAll -> FinalUpdate 순서로 실행합니다.
//   - 현재는 실행 단계의 경계만 정의합니다.
//   - 실행 참여 대상과 등록·해제 시점, 단계별 실행 순서는
//     Scheduler 계약과 함께 후속 구현합니다.
//   - 플랫폼 시간을 직접 조회하지 않고 GameRuntime이 전달한 delta time만 사용합니다.
//
// 책임:
//   - 현재 World-local simulation 상태 소유
//   - 공유 ComponentData 정본 소유
//   - 소속 WorldObject의 비소유 참조와 World-local 표현 연결
//   - Runtime이 요청한 한 번의 simulation step 수행
//   - PreUpdate -> ProcessAll -> FinalUpdate 실행 단계 경계 보장
//   - Snapshot 대상이 되는 논리 상태 유지
//
// 비책임:
//   - 자체 Run loop 소유
//   - delta time 산출과 Tick 호출 정책
//   - 실행 참여 대상의 Scheduler 등록·해제 정책
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

	explicit World(GameRuntime& owningRuntime);
	~World();

	void Reset();
	void Tick(double deltaSeconds);

	// World Loop Phases
	void PreUpdate(double deltaSeconds);
	void ProcessAll(double deltaSeconds);
	void FinalUpdate(double deltaSeconds);

	uint64 simulationTick = 0;
	double simulationTime = 0.0;

	GameRuntime& owningRuntime;
	Chunk worldOrigin = {};
	std::vector<WorldObject*> worldObjects;
	ComponentDataStorage<Transform, ObjectHandle> transforms;
};

template<typename WorldObjectType, typename... ArgumentTypes>
WorldObjectType* World::Spawn(const Transform& initialTransform, ArgumentTypes&&... arguments)
{
	WorldObjectType* worldObject = Object::NewObject<WorldObjectType>(std::forward<ArgumentTypes>(arguments)...);

	if (worldObject == nullptr)
	{
		return nullptr;
	}

	if (owningRuntime.Admit(*worldObject) == false || Join(*worldObject, initialTransform) == false)
	{
		worldObject->Destroy();

		return nullptr;
	}

	return worldObject;
}
