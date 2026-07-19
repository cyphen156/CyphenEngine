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
// World는 WorldObject의 메모리와 수명을 소유하지 않습니다.
// World-local ComponentData 정본과 소속 WorldObject의 비소유 참조를 유지합니다.
//
// Join:
//   - 생성된 WorldObject를 현재 World의 논리 공간에 합류시킵니다.
//   - WorldObject의 Transform 정본을 생성합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 연결합니다.
//   - Update 또는 System 실행 참여를 자동으로 결정하지 않습니다.
//
// Leave:
//   - WorldObject의 Transform 정본을 제거합니다.
//   - WorldObject의 비소유 참조와 World 소속 관계를 해제합니다.
//   - WorldObject의 메모리와 Object 수명은 변경하지 않습니다.
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
//   - WorldObject 생성과 수명 관리
//   - ObjectHandle 발급과 Object 메모리 소유
//   - Component 부착과 System 참여 정책
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

	bool Join(WorldObject& worldObject, const Transform& initialTransform);
	bool Leave(WorldObject& worldObject);

	void Reset();
	void Tick(double deltaSeconds);

	// World Loop Phases
	void PreUpdate();
	void ProcessAll(double deltaSeconds);
	void FinalUpdate();

	bool TryGetTransform(ObjectHandle objectHandle, Transform& outTransform) const;

	uint64 simulationTick = 0;
	double simulationTime = 0.0;

	Chunk worldOrigin = {};
	ComponentDataStorage<Transform, ObjectHandle> transforms;
	std::vector<WorldObject*> worldObjects;
};
