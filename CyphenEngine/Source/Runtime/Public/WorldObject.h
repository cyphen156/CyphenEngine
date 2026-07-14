#pragma once

#include "Core/Public/Handle.h"
#include "Core/Public/Math/Transform.h"
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
// WorldObject는 자신의 Transform 슬롯을 Handle로 식별합니다.
// ============================================================================

class WorldObject : public GameObject
{
public:
	~WorldObject() override;

	bool TryGetTransform(Transform& outTransform) const;
	bool SetTransform(const Transform& transform);

	Handle<Transform> GetTransformHandle() const;

private:
	friend class World;

	WorldObject(
		ObjectHandle objectHandle,
		ComponentDataStorage<Transform>& transformDataStorage,
		Handle<Transform> transformDataHandle);

	ComponentDataStorage<Transform>* transformStorage = nullptr;
	Handle<Transform> transformHandle;
};
