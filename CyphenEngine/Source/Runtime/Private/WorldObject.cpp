#include "pch.h"

#include "Runtime/Public/WorldObject.h"

WorldObject::WorldObject(
	ObjectHandle objectHandle,
	ComponentDataStorage<Transform>& transformDataStorage,
	Handle<Transform> transformDataHandle)
	: GameObject(objectHandle)
	, transformStorage(&transformDataStorage)
	, transformHandle(transformDataHandle)
{
}

WorldObject::~WorldObject()
{
	// 부착 Component가 소멸 과정에서 Transform을 사용할 수 있도록
	// Component를 먼저 파괴한 뒤 Transform 슬롯을 반환합니다.
	ClearComponents();

	if (transformStorage != nullptr && transformHandle.IsSet())
	{
		transformStorage->Remove(transformHandle);
	}

	transformStorage = nullptr;
	transformHandle = {};
}

bool WorldObject::TryGetTransform(Transform& outTransform) const
{
	if (transformStorage == nullptr)
	{
		return false;
	}

	return transformStorage->TryGet(transformHandle, outTransform);
}

bool WorldObject::SetTransform(const Transform& transform)
{
	if (transformStorage == nullptr)
	{
		return false;
	}

	return transformStorage->Set(transformHandle, transform);
}

Handle<Transform> WorldObject::GetTransformHandle() const
{
	return transformHandle;
}
