#pragma once

#include "Runtime/Public/World.h"

class CyphenEngine;
class WorldObject;

// ============================================================================
// GameRuntime
// ----------------------------------------------------------------------------
// 하나의 게임 실행 인스턴스를 나타내는 Runtime 컨텍스트입니다.
//
// Initialize:
//   - 소유 World를 초기 상태로 구성합니다.
//
// Tick:
//   - 하나의 Runtime simulation step을 다음 다섯 단계로 실행합니다.
//
//     1. GlobalPreUpdate
//        Runtime 전체 범위의 OOP 선행 행위를 실행합니다.
//
//     2. ProcessBeforeWorldTicks
//        World들이 실행되기 전에 필요한 Runtime-global System 처리를 수행합니다.
//
//     3. World Tick
//        각 World의 PreUpdate -> ProcessAll -> FinalUpdate를 실행합니다.
//
//     4. ProcessAfterWorldTicks
//        모든 World의 실행 결과를 사용하는 Runtime-global System 처리를 수행합니다.
//
//     5. GlobalFinalUpdate
//        확정된 Runtime 전체 결과를 사용하는 OOP 후행 행위를 실행합니다.
//
//   - 현재는 하나의 World를 소유하지만 다중 World로 확장되어도
//     다섯 단계의 실행 경계와 순서는 변경하지 않습니다.
//   - 현재 global 실행 목록과 System은 구현하지 않으며 실행 단계의 경계만 확정합니다.
//
// Shutdown:
//   - 소유 World를 초기 상태로 되돌립니다.
//
// 책임:
//   - World 수명 소유
//   - World Tick 호출과 delta time 전달
//   - Runtime-global 단계와 World-local 단계의 실행 순서 조율
//   - 하나의 Runtime에 속한 여러 World를 포괄할 수 있는 실행 경계 제공
//
// 비책임:
//   - Engine 최상위 Run loop와 플랫폼 이벤트 처리
//   - World 내부 simulation 규칙 구현
//   - World-local Update와 System 실행 참여 정책
//   - Renderer와 Editor 기능 소유
//   - Runtime 다중 인스턴스 구성 정책
// ============================================================================

class GameRuntime final
{
public:
	const World& GetWorld() const;
	bool IsInitialized() const;

private:
	friend class CyphenEngine;
	friend class WorldObject;

	bool Initialize();
	void Tick(double deltaSeconds);

	void GlobalUpdate(double deltaSeconds);
	void ProcessBeforeWorldTicks(double deltaSeconds);
	void ProcessAfterWorldTicks(double deltaSeconds);
	void GlobalFinalUpdate(double deltaSeconds);

	void Shutdown();

	World world;
	bool isInitialized = false;
};
