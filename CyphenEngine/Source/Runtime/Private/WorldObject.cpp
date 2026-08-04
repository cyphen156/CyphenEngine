#include "pch.h"

#ifdef _DEBUG
#include <cassert>
#endif

#include "Runtime/Public/WorldObject.h"
#include "Runtime/Public/World.h"

Transform WorldObject::GetTransform() const
{
	Transform transform = Transform::Identity();

	if (world == nullptr)
	{
#ifdef _DEBUG
		PRINT_DEBUG_OUTPUT("[WorldObject] World에 합류한 이후에 Transform을 조회해야 합니다.\n");

		assert(world != nullptr);
#endif
		return transform;
	}

	if (world->TryGetTransform(GetHandle(), transform) == false)
	{
#ifdef _DEBUG
		PRINT_DEBUG_OUTPUT("[WorldObject] World에서 Transform을 찾을 수 없습니다.\n");

		assert(false);
#endif
	}

	return transform;
}

World* WorldObject::GetWorld()
{
	return world;
}

const World* WorldObject::GetWorld() const
{
	return world;
}

WorldObject::WorldObject(ObjectHandle objectHandle)
	: GameObject(objectHandle)
{
}

WorldObject::WorldObject(ObjectHandle objectHandle, UpdateParticipation updateParticipation)
	: GameObject(objectHandle, updateParticipation)
{
}

WorldObject::~WorldObject() = default;

bool WorldObject::IsUpdateExecutionAnchor(UpdateParticipation participation) const
{
	if (world == nullptr)
	{
		return false;
	}

	return 
		participation == UpdateParticipation::Update ||
		participation == UpdateParticipation::FinalUpdate;
}

bool WorldObject::OnDestroy()
{
	if (world != nullptr && world->Leave(*this) == false)
	{
		return false;
	}

	return GameObject::OnDestroy();
}
