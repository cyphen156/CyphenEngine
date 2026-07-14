#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/Math/Transform.h"
#include "Runtime/Public/Chunk.h"
#include "Runtime/Public/ComponentDataStorage.h"

class GameRuntime;

// ============================================================================
// World
// ----------------------------------------------------------------------------
// Object와 System이 살아가는 독립된 논리 시뮬레이션 공간입니다.
//
// Tick:
//   - GameRuntime이 simulation을 전진시킬 때 호출합니다.
//   - 한 번의 호출은 한 번의 World simulation step을 의미합니다.
//   - World가 자체 반복문을 실행하거나 플랫폼 시간을 조회하지 않습니다.
//
// 책임:
//   - 현재 World-local simulation 상태 소유
//   - 공유 ComponentData 정본 소유
//   - Runtime이 요청한 한 번의 simulation step 수행
//
// 비책임:
//   - 자체 Run loop 소유
//   - delta time 산출과 Tick 호출 정책
//   - Render Frame 생성과 Renderer 연결
//   - WorldObject 생성과 수명 관리
//   - Component 부착과 System enroll 정책
// ============================================================================

class World final
{
public:
	World() = default;
	~World() = default;

	World(const World&) = delete;
	World& operator=(const World&) = delete;
	World(World&&) = delete;
	World& operator=(World&&) = delete;

	uint64 GetSimulationTick() const;
	double GetSimulationTime() const;

private:
	friend class GameRuntime;

	void Reset();
	void Tick(double deltaSeconds);

	uint64 simulationTick = 0;
	double simulationTime = 0.0;

	Chunk worldOrigin = {};
	ComponentDataStorage<Transform> transforms;
};
