#pragma once

#include "Core/Public/Math/Transform.h"
#include "Runtime/Public/GameObject.h"

class GameRuntime;
class World;
class ObjectManager;

// ============================================================================
// WorldObjectInstantiateParams
// ----------------------------------------------------------------------------
// WorldObject가 표현될 논리 World와 최초 Transform을 전달하는 파라미터 그룹입니다.
//
// DefaultWorld는 지정한 GameRuntime의 기본 World를 선택합니다.
// TargetWorld는 Runtime의 기본값을 거치지 않고 명시적인 World를 선택합니다.
// 파라미터는 Object 생성이 아니라 생성된 WorldObject의 World 소속에만 사용됩니다.
// ============================================================================

class WorldObjectInstantiateParams final
{
public:
	static WorldObjectInstantiateParams DefaultWorld(
		GameRuntime& runtime,
		const Transform& initialTransform = Transform::Identity());

	static WorldObjectInstantiateParams TargetWorld(
		World& targetWorld,
		const Transform& initialTransform = Transform::Identity());

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
// World에 소속되어 Transform을 갖는 OOP 공간 객체입니다.
//
// WorldObject는 생성 시 ObjectHandle을 부여받지만 생성과 World 소속은
// 서로 다른 단계입니다. World에 소속되기 전까지 world는 nullptr입니다.
//
// 일반 OOP Component composition은 GameObject가 소유합니다.
// World에 소속된 이후에는 ObjectHandle을 통해 해당 World의 Transform 표현을 조회합니다.
// Transform의 현재 StorageSlot은 World와 ComponentDataStorage만 관리합니다.
// ============================================================================

class WorldObject : public GameObject
{
public:
	bool Instantiate(const WorldObjectInstantiateParams& parameters);

	Transform GetTransform() const;
	const World* GetWorld() const;

protected:
	explicit WorldObject(
		ObjectHandle objectHandle,
		bool participatesInWorldUpdate = true);
	~WorldObject() override;

private:
	friend class ObjectManager;
	friend class World;

	World* world = nullptr;
	bool participatesInWorldUpdate = true;
};
