#pragma once

#include <vector>

class CyphenEngine;
class GameObject;
class World;

// ============================================================================
// GameRuntime
// ----------------------------------------------------------------------------
// 하나의 게임 실행 인스턴스를 나타내는 Runtime 컨텍스트입니다.
//
// Initialize:
//   - Runtime 실행 상태를 초기화합니다.
//   - World를 자동으로 생성하지 않습니다.
//
// CreateWorld / DestroyWorld:
//   - Runtime에 속한 독립 논리 공간을 명시적으로 생성하고 파괴합니다.
//   - Runtime은 World를 0개 이상 소유할 수 있습니다.
//   - 모든 World는 생성 시점부터 현재 Runtime에 속하며 무소속 상태를 허용하지 않습니다.
//
// Admit:
//   - 이미 생성된 루트 GameObject의 Runtime 소속을 현재 Runtime으로
//     확정합니다.
//   - 다른 Runtime에 이미 속한 GameObject의 전이를 허용하지 않습니다.
//   - 자식 GameObject와 Component의 실행 참여는 처리하지 않습니다.
//
// Destroy:
//   - GameObject의 다형적 Destroy 경로를 통해서만 호출합니다.
//   - GameObject가 공통 Object 파괴를 요청하기 전에 Runtime 소속을 해제합니다.
//   - 실제 Object 파괴 요청은 GameObject::Destroy가 담당합니다.
//   - 살아 있는 미소속 GameObject로 되돌리는 Leave 또는 Detach는 제공하지 않습니다.
//
// Tick:
//   - 하나의 Runtime simulation step을 다음 다섯 단계로 실행합니다.
//
//     1. GlobalUpdate
//        Runtime 전체 범위의 OOP 선행 행위를 실행합니다.
//
//     2. ProcessBeforeWorldTicks
//        World들이 실행되기 전에 필요한 Runtime-global System 처리를 수행합니다.
//
//     3. World Tick
//        현재 Tick에 참여하는 World의 PreUpdate -> ProcessAll -> FinalUpdate를 실행합니다.
//
//     4. ProcessAfterWorldTicks
//        모든 World의 실행 결과를 사용하는 Runtime-global System 처리를 수행합니다.
//
//     5. GlobalFinalUpdate
//        확정된 Runtime 전체 결과를 사용하는 OOP 후행 행위를 실행합니다.
//
//   - World의 Runtime 소속과 Tick 참여는 별개의 관계입니다.
//   - 현재는 World Tick 참여 정책을 분리하지 않으며 소유 World를 순서대로 실행합니다.
//   - 현재 global 실행 목록과 System은 구현하지 않으며 실행 단계의 경계만 확정합니다.
//
// Shutdown:
//   - 소속 GameObject와 명시적으로 생성된 모든 World를 파괴합니다.
//
// 책임:
//   - 소속 GameObject의 Runtime 관계와 파괴 이탈 경로 관리
//   - 명시적으로 생성된 World의 정체성과 수명 소유
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
	bool Admit(GameObject& gameObject);

	bool IsInitialized() const;

private:
	friend class CyphenEngine;
	friend class GameObject;

	GameRuntime();
	~GameRuntime() = default;

	bool Initialize();
	void Shutdown();

	void Tick(double deltaSeconds);

	void GlobalUpdate(double deltaSeconds);
	void ProcessBeforeWorldTicks(double deltaSeconds);
	void ProcessAfterWorldTicks(double deltaSeconds);
	void GlobalFinalUpdate(double deltaSeconds);

	World* CreateWorld();
	bool DestroyWorld(World& world);

	bool Destroy(GameObject& gameObject);

	bool isInitialized = false;

	std::vector<World*> worlds;
	std::vector<GameObject*> gameObjects;
};
