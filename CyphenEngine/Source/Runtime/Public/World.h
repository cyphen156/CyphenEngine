#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/Math/Transform.h"
#include "Runtime/Public/Chunk.h"
#include "Runtime/Public/ComponentDataStorage.h"
#include "Runtime/Public/ObjectManager.h"

class GameRuntime;
class WorldObject;

// ============================================================================
// World
// ----------------------------------------------------------------------------
// Object와 System이 살아가는 독립된 논리 시뮬레이션 공간입니다.
//
// Tick:
//   - GameRuntime이 simulation을 전진시킬 때 호출합니다.
//   - 한 번의 호출은 한 번의 World simulation step을 의미합니다.
//   - PreUpdate -> ProcessAll -> FinalUpdate 순서로 실행합니다.
//   - PreUpdate는 GameObject::Update를, FinalUpdate는 GameObject::FinalUpdate를
//     호출합니다.
//   - 플랫폼 시간을 직접 조회하지 않고 GameRuntime이 전달한 delta time만 사용합니다.
//
// 책임:
//   - 현재 World-local simulation 상태 소유
//   - 공유 ComponentData 정본 소유
//   - Runtime이 요청한 한 번의 simulation step 수행
//   - 소속 WorldObject의 비소유 참조와 World-local 표현 연결
//   - World-local Update / FinalUpdate 참여 순서 소유
//   - Snapshot 대상이 되는 논리 상태 유지
//
// 비책임:
//   - 자체 Run loop 소유
//   - delta time 산출과 Tick 호출 정책
//   - Render Frame 생성과 Renderer 연결
//   - WorldObject 생성과 수명 관리
//   - ObjectHandle 발급과 Object 메모리 소유
//   - Component 부착과 System enroll 정책
// ============================================================================

class World final
{
public:
	World() = default;
	~World();

	World(const World&) = delete;
	World& operator=(const World&) = delete;
	World(World&&) = delete;
	World& operator=(World&&) = delete;

	uint64 GetSimulationTick() const;
	double GetSimulationTime() const;

private:
	friend class GameRuntime;
	friend class WorldObject;

	void Reset();
	void Tick(double deltaSeconds);
	void PreUpdate(double deltaSeconds);
	void ProcessAll(double deltaSeconds);
	void FinalUpdate(double deltaSeconds);

	bool Enroll(WorldObject& object, const Transform& initialTransform);
	bool Unroll(WorldObject& object);

	bool TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const;

	uint64 simulationTick = 0;
	double simulationTime = 0.0;

	Chunk worldOrigin = {};
	ComponentDataStorage<Transform, ObjectHandle> transforms;
	std::vector<WorldObject*> worldObjects;
	std::vector<WorldObject*> worldUpdates;
};
