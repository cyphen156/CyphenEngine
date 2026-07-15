#pragma once

#include "Core/Public/Math/Transform.h"
#include "Core/Public/StorageSlot.h"
#include "Runtime/Public/ComponentDataStorage.h"
#include "Runtime/Public/GameObject.h"

class World;

// ============================================================================
// WorldObject
// ----------------------------------------------------------------------------
// World에 소속되어 Transform을 갖는 OOP 공간 객체입니다.
//
// 일반 OOP Component는 GameObject가 직접 소유합니다.
// Transform 값은 World의 ComponentDataStorage에 저장되며,
// WorldObject는 자신의 Transform 저장 위치를 StorageSlot으로 식별합니다.
// ============================================================================

class WorldObject : public GameObject
{
public:
	~WorldObject() override;

	//World& GetWorld();
	const World& GetWorld() const;

	//Transform GetTransform() const;

private:
	friend class World;

	WorldObject() = default;

	World* world = nullptr;
	Transform m_transform = {};
};
