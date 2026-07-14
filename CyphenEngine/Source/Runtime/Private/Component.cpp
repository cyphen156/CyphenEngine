#include "pch.h"

#include "Runtime/Public/Component.h"

Component::Component(ObjectHandle objectHandle)
	: Object(objectHandle)
{
}

Component::~Component() = default;

GameObject* Component::GetOwner()
{
	return owner;
}

const GameObject* Component::GetOwner() const
{
	return owner;
}

void Component::BindOwner(GameObject* newOwner)
{
	owner = newOwner;
}
