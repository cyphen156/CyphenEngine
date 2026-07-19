#pragma once

#include "Core/Public/Math/Transform.h"
#include "Runtime/Public/GameObject.h"

class GameRuntime;
class World;
class ObjectManager;

// ============================================================================
// WorldObjectInstantiateParams
// ----------------------------------------------------------------------------
// WorldObject가 표현될 World와 최초 Transform을 전달하는 파라미터 그룹입니다.
//
// DefaultWorld는 지정한 GameRuntime의 기본 World를 선택합니다.
// TargetWorld는 Runtime의 기본값을 거치지 않고 명시적인 World를 선택합니다.
//
// 파라미터는 Object 생성에 사용하지 않습니다.
// 생성된 WorldObject가 최초 World에 합류할 때만 사용합니다.
// ============================================================================

class WorldObjectInstantiateParams final
{
public:
	static WorldObjectInstantiateParams DefaultWorld(
		GameRuntime& runtime, const Transform& initialTransform = Transform::Identity());

	static WorldObjectInstantiateParams TargetWorld(
		World& targetWorld, const Transform& initialTransform = Transform::Identity());

private:
	friend class WorldObject;

	WorldObjectInstantiateParams() = default;

	GameRuntime* runtime = nullptr;
	World* targetWorld = nullptr;
	Transform initialTransform = Transform::Identity();
};

// ============================================================================
// WorldObject
// ----------------------------------------------------------------------------
// World에 합류하여 Transform을 갖는 OOP 공간 객체입니다.
//
// WorldObject는 ObjectHandle을 부여받아 먼저 생성할 수 있습니다.
// 생성 직후에는 아직 World에 합류하지 않았으며 world는 nullptr입니다.
//
// Instantiate는 생성된 WorldObject가 최초 World에 합류하도록 요청합니다.
// 실제 World-local 상태 변경은 대상 World의 Join이 담당합니다.
//
// World에 합류한 WorldObject는 반드시 Transform을 가집니다.
// 아직 합류하지 않은 WorldObject는 Transform을 조회할 수 없습니다.
//
// Destroy는 현재 World에 합류한 상태라면 먼저 Leave를 요청한 뒤
// 공통 Object 파괴 요청 경로에 합류합니다.
//
// World membership은 Update Scheduler 참여와 별개의 관계입니다.
// Instantiate는 World 표현을 구성하지만 Update 실행 참여를 자동으로 결정하지 않습니다.
//
// WorldObject는 소속 World를 비소유 포인터로 저장합니다.
// World의 수명을 소유하거나 연장하지 않습니다.
// ============================================================================

class WorldObject : public GameObject
{
public:
	bool Instantiate(const WorldObjectInstantiateParams& parameters);
	bool Destroy() override;

	Transform GetTransform() const;
	const World* GetWorld() const;

protected:
	explicit WorldObject(ObjectHandle objectHandle);
	~WorldObject() override;

private:
	friend class ObjectManager;
	friend class World;

	World* world = nullptr;
};
