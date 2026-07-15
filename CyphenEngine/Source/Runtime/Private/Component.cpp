#include "pch.h"

#include "Runtime/Public/Component.h"
#include "Runtime/Public/GameObject.h"

GameObject* Component::GetOwner()
{
	return owner;
}

const GameObject* Component::GetOwner() const
{
	return owner;
}

Component::Component(ObjectHandle objectHandle, GameObject& newOwner)
	: Object(objectHandle)
	, owner(&newOwner)
{
}

Component::~Component()
{
	if (owner == nullptr)
	{
		return;
	}

	GameObject* previousOwner = owner;

	owner = nullptr;
	previousOwner->DetachComponent(this);
}
