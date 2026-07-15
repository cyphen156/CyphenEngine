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

GameObject::~GameObject()
{
	ClearComponents();
}

void GameObject::ClearComponents()
{
	while (components.empty() == false)
	{
		Component* component = components.back();
		components.pop_back();

		component->owner = nullptr;
		component->Destroy();
	}
}

void GameObject::DetachComponent(Component* component)
{
	std::vector<Component*>::iterator iterator;

	for (iterator = components.begin();
		iterator != components.end();
		++iterator)
	{
		if (*iterator != component)
		{
			continue;
		}

		components.erase(iterator);

		return;
	}
}
