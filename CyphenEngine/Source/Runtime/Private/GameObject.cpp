#include "pch.h"

#include <utility>

#include "Runtime/Public/GameObject.h"

GameObject::GameObject(ObjectHandle objectHandle)
	: Object(objectHandle)
{
}

GameObject::~GameObject()
{
	ClearComponents();
}

Component* GameObject::AddComponent(std::unique_ptr<Component> component)
{
	if (component == nullptr)
	{
		return nullptr;
	}

	if (component->GetHandle().IsSet() == false)
	{
		return nullptr;
	}

	if (component->GetOwner() != nullptr)
	{
		return nullptr;
	}

	if (FindComponent(component->GetHandle()) != nullptr)
	{
		return nullptr;
	}

	Component* addedComponent = component.get();

	components.push_back(std::move(component));
	addedComponent->BindOwner(this);

	return addedComponent;
}

bool GameObject::RemoveComponent(ObjectHandle componentHandle)
{
	std::vector<std::unique_ptr<Component>>::iterator iterator;
	for (iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->GetHandle() != componentHandle)
		{
			continue;
		}

		// Component 소멸 중에는 소유 GameObject가 아직 살아 있습니다.
		components.erase(iterator);
		return true;
	}

	return false;
}

Component* GameObject::FindComponent(ObjectHandle componentHandle)
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (component->GetHandle() == componentHandle)
		{
			return component.get();
		}
	}

	return nullptr;
}

const Component* GameObject::FindComponent(ObjectHandle componentHandle) const
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (component->GetHandle() == componentHandle)
		{
			return component.get();
		}
	}

	return nullptr;
}

uint32 GameObject::GetComponentCount() const
{
	return static_cast<uint32>(components.size());
}

void GameObject::ClearComponents()
{
	while (components.empty() == false)
	{
		components.pop_back();
	}
}
