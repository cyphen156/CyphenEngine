#pragma once

#include <vector>

#include "Runtime/Public/Object.h"
#include "Runtime/Public/UpdateManager.h"
#include "Runtime/Public/UpdateParticipation.h"

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
//   - 각 World의 World-local Function Group은 UpdateManager가 생성하고 파괴합니다.
//   - GetWorldCount / GetWorld는 소유한 World를 생성 순서대로 조회하는
//     비소유 접근자입니다.
//
// Spawn:
//   - 새 GameObject를 생성하고 현재 Runtime에 Admit하는 합성 생성 경로입니다.
//   - Admit에 실패하면 생성된 GameObject를 파괴하고 nullptr을 반환합니다.
//
// Admit:
//   - 이미 생성된 GameObject 서브트리의 Runtime 소속을 현재 Runtime으로
//     확정합니다.
//   - 다른 Runtime에 이미 속한 GameObject의 전이를 허용하지 않습니다.
//   - Component 아래의 GameObject는 독립적인 Runtime 참여 루트가 될 수 없습니다.
//   - 적용 전에 서브트리 전체의 충돌을 검사하고, 포함된 모든 GameObject를
//     평면 gameObjects 장부에 등록합니다.
//   - 이미 같은 Runtime에 참여한 GameObject는 중복 등록하지 않습니다.
//   - 각 GameObject가 공급한 자신과 직접 Component의 UpdateFunction을 등록합니다.
//     Update / FinalUpdate는 World anchor가 World에 합류한 경우에만 등록합니다.
//   - 부분 실패 시 이번 호출이 등록한 접미부만 역순으로 롤백합니다.
//
// Leave:
//   - GameObject 서브트리 전체의 Runtime 참여를 해제합니다.
//   - 포함된 WorldObject는 자식부터 각자의 World를 먼저 떠납니다.
//   - 모든 실행 참여를 해제한 뒤 평면 장부 제거를 커밋합니다.
//
// RegisterUpdateFunction / UnregisterUpdateFunction:
//   - GameObject가 공급한 UpdateFunction 목록을 실행 그룹에 반영합니다.
//   - GlobalUpdate / GlobalFinalUpdate는 Runtime-global 그룹을 사용합니다.
//   - Update / FinalUpdate는 Outer 체인의 World anchor로 대상 World를 결정합니다.
//   - 부분 실패 시 이번 요청에서 변경한 항목만 역순으로 원복합니다.
//
// Destroy:
//   - 파괴 가능한 살아 있는 GameObject를 Leave시킨 뒤 공통 Object 파괴를
//     요청합니다.
//   - Leave조차 수행할 수 없는 손상 객체의 강제 수집은 후속 ObjectManager
//     또는 GC의 별도 우회 경로로 분리합니다.
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
//        현재 Tick에 참여하는 World의 Update -> ProcessAll -> FinalUpdate를 실행합니다.
//
//     4. ProcessAfterWorldTicks
//        모든 World의 실행 결과를 사용하는 Runtime-global System 처리를 수행합니다.
//
//     5. GlobalFinalUpdate
//        확정된 Runtime 전체 결과를 사용하는 OOP 후행 행위를 실행합니다.
//
//   - World의 Runtime 소속과 Tick 참여는 별개의 관계입니다.
//   - 현재는 World Tick 참여 정책을 분리하지 않으며 소유 World를 순서대로 실행합니다.
//   - Runtime-global Function Group은 등록된 OOP 실행 함수만 순회합니다.
//   - Runtime-global System은 아직 구현하지 않습니다.
//
// Shutdown:
//   - 소속 GameObject와 명시적으로 생성된 모든 World를 파괴합니다.
//
// 책임:
//   - 소속 GameObject의 Runtime 관계와 파괴 이탈 경로 관리
//   - UpdateManager를 통한 Runtime-global 및 World-local Function Group 수명 관리
//   - GameObject가 공급한 UpdateFunction의 등록·해제와 World anchor 라우팅
//   - Runtime-global Function Group의 직접 실행
//   - 명시적으로 생성된 World의 정체성과 수명 소유
//   - World Tick 호출과 delta time 전달
//   - Runtime-global 단계와 World-local 단계의 실행 순서 조율
//   - 하나의 Runtime에 속한 여러 World를 포괄할 수 있는 실행 경계 제공
//
// 비책임:
//   - Engine 최상위 Run loop와 플랫폼 이벤트 처리
//   - World 내부 simulation 규칙과 System 실행 구현
//   - GameObject와 Component의 UpdateFunction 생성
//   - Renderer와 Editor 기능 소유
//   - Runtime 다중 인스턴스 구성 정책
// ============================================================================

class GameRuntime final
{
public:
	template<typename GameObjectType, typename... ArgumentTypes>
	GameObjectType* Spawn(ArgumentTypes&&... arguments);

	bool Admit(GameObject& gameObject);
	bool Leave(GameObject& gameObject);

	bool RegisterUpdateFunction(GameObject& gameObject, 
		UpdateParticipation participation, const std::vector<UpdateFunction>& updateFunctionList);
	bool UnregisterUpdateFunction(GameObject& gameObject, 
		UpdateParticipation participation, const std::vector<UpdateFunction>& updateFunctionList);

	bool IsInitialized() const;

	uint32 GetWorldCount() const;
	World* GetWorld(uint32 worldIndex);

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

	UpdateManager updateManager;

	std::vector<World*> worlds;
	std::vector<GameObject*> gameObjects;
};

template<typename GameObjectType, typename... ArgumentTypes>
GameObjectType* GameRuntime::Spawn(ArgumentTypes&&... arguments)
{
	GameObjectType* gameObject = Object::NewObject<GameObjectType>(std::forward<ArgumentTypes>(arguments)...);

	if (gameObject == nullptr)
	{
		return nullptr;
	}

	if (Admit(*gameObject) == false)
	{
		gameObject->Destroy();

		return nullptr;
	}

	return gameObject;
}
