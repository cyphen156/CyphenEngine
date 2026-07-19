#include "pch.h"

#include "Runtime/Public/GameObject.h"

Component* GameObject::FindComponent(ObjectHandle componentHandle)
{
	for (Component* component : components)
	{
		if (component->GetHandle() == componentHandle)
		{
			return component;
		}
	}

	return nullptr;
}

const Component* GameObject::FindComponent(ObjectHandle componentHandle) const
{
	for (const Component* component : components)
	{
		if (component->GetHandle() == componentHandle)
		{
			return component;
		}
	}

	return nullptr;
}

uint32 GameObject::GetComponentCount() const
{
	return static_cast<uint32>(components.size());
}

GameObject::GameObject(ObjectHandle objectHandle)
	: Object(objectHandle)
{
}

GameObject::~GameObject() = default;

bool GameObject::DetachComponent(Component* component)
{
	if (component == nullptr)
	{
		return false;
	}

	std::vector<Component*>::iterator iterator;
	for (iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if (*iterator != component)
		{
			continue;
		}

		if (DetachSubObject(*component) == false)
		{
			return false;
		}

		components.erase(iterator);

		return true;
	}

	return false;
}
