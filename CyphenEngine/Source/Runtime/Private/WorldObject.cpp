#include "pch.h"

#ifdef _DEBUG
#include <cassert>
#endif

#include "Runtime/Public/GameRuntime.h"
#include "Runtime/Public/World.h"
#include "Runtime/Public/WorldObject.h"

WorldObjectInstantiateParams WorldObjectInstantiateParams::DefaultWorld(
	GameRuntime& runtime, const Transform& initialTransform)
{
	WorldObjectInstantiateParams parameters;
	parameters.runtime = &runtime;
	parameters.initialTransform = initialTransform;
	return parameters;
}

WorldObjectInstantiateParams WorldObjectInstantiateParams::TargetWorld(
	World& targetWorld, const Transform& initialTransform)
{
	WorldObjectInstantiateParams parameters;
	parameters.targetWorld = &targetWorld;
	parameters.initialTransform = initialTransform;
	return parameters;
}

bool WorldObject::Instantiate(const WorldObjectInstantiateParams& parameters)
{
	if (world != nullptr)
	{
		return false;
	}

	World* targetWorld = parameters.targetWorld;

	if (targetWorld == nullptr)
	{
		if (parameters.runtime == nullptr || parameters.runtime->IsInitialized() == false)
		{
			return false;
		}

		targetWorld = &parameters.runtime->world;
	}

	return targetWorld->Join(*this, parameters.initialTransform);
}

bool WorldObject::Destroy()
{
	if (world != nullptr && world->Leave(*this) == false)
	{
		return false;
	}

	return Object::Destroy();
}

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

const World* WorldObject::GetWorld() const
{
	return world;
}

WorldObject::WorldObject(ObjectHandle objectHandle)
	: GameObject(objectHandle)
{
}

WorldObject::~WorldObject() = default;
