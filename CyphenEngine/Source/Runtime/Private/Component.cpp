#include "pch.h"

#include "Runtime/Public/Component.h"
#include "Runtime/Public/GameObject.h"

GameObject* Component::GetOwner()
{
	return static_cast<GameObject*>(GetOuter());
}

const GameObject* Component::GetOwner() const
{
	return static_cast<const GameObject*>(GetOuter());
}

bool Component::Destroy()
{
	GameObject* owner = GetOwner();

	if (owner != nullptr && owner->DetachComponent(this) == false)
	{
		return false;
	}

	return Object::Destroy();
}

Component::Component(ObjectHandle objectHandle)
	: Object(objectHandle)
{
}

Component::~Component() = default;

bool Component::CanAttachTo(const Object& outer) const
{
	return dynamic_cast<const GameObject*>(&outer) != nullptr;
}
