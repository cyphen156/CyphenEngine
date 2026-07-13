#pragma once

#include "Runtime/Public/World.h"

class CyphenEngine;

// ============================================================================
// GameRuntime
// ----------------------------------------------------------------------------
// 하나의 게임 실행 인스턴스를 나타내는 Runtime 컨텍스트입니다.
//
// Initialize:
//   - 소유 World를 초기 상태로 구성합니다.
//
// Tick:
//   - CyphenEngine이 전달한 delta time으로 World를 한 단계 전진시킵니다.
//
// Shutdown:
//   - 소유 World를 초기 상태로 되돌립니다.
//
// 책임:
//   - World 수명 소유
//   - World Tick 호출과 delta time 전달
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

	bool Initialize();
	void Tick(double deltaSeconds);
	void Shutdown();

	World world;
	bool isInitialized = false;
};
