#include "pch.h"

#include "Runtime/Public/Component.h"
#include "Runtime/Public/GameObject.h"
#include "Runtime/Public/GameRuntime.h"

void GameObject::SetActive(bool activationFlag)
{
	isActive = activationFlag;
}

bool GameObject::IsActive() const
{
	return isActive;
}

bool GameObject::Destroy()
{
	if (runtime != nullptr && runtime->Destroy(*this) == false)
	{
		return false;
	}

	return Object::Destroy();
}

void GameObject::GlobalUpdate(double deltaSeconds)
{
}

void GameObject::Update(double deltaSeconds)
{
}

void GameObject::FinalUpdate(double deltaSeconds)
{
}

void GameObject::GlobalFinalUpdate(double deltaSeconds)
{
}

bool GameObject::HasUpdateParticipation(UpdateParticipation participation) const
{
	return (static_cast<uint8>(updateParticipation) & static_cast<uint8>(participation)) != 0;
}

uint32 GameObject::GetComponentCount() const
{
	uint32 componentCount = 0;
	const uint32 subObjectCount = GetSubObjectCount();

	for (uint32 index = 0; index < subObjectCount; ++index)
	{
		if (dynamic_cast<const Component*>(GetSubObject(index)) != nullptr)
		{
			++componentCount;
		}
	}

	return componentCount;
}

const GameRuntime* GameObject::GetGameRuntime() const
{
	return runtime;
}

GameObject::GameObject(ObjectHandle objectHandle)
	: GameObject(objectHandle, UpdateParticipation::None)
{
}

GameObject::GameObject(ObjectHandle objectHandle, UpdateParticipation updateParticipationValue)
	: Object(objectHandle),
	updateParticipation(updateParticipationValue)
{
}

GameObject::~GameObject() = default;
