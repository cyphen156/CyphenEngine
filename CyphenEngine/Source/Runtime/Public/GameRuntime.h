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
//   - GlobalPreUpdate 이후 소유 World를 한 단계 전진시킵니다.
//   - 모든 World-local 처리가 끝난 뒤 GlobalFinalUpdate를 수행합니다.
//   - 현재 global 실행 목록은 없으며 두 phase는 실행 순서 경계만 확정합니다.
//
// Shutdown:
//   - 소유 World를 초기 상태로 되돌립니다.
//
// 책임:
//   - World 수명 소유
//   - World Tick 호출과 delta time 전달
//   - Runtime-global phase와 World-local phase의 실행 순서 조율
//
// 비책임:
//   - Engine 최상위 Run loop와 플랫폼 이벤트 처리
//   - World 내부 simulation 규칙 구현
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
	void GlobalPreUpdate(double deltaSeconds);
	void GlobalFinalUpdate(double deltaSeconds);
	void Shutdown();

	World world;
	bool isInitialized = false;
};
