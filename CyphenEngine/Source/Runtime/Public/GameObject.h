#pragma once

#include <memory>
#include <vector>

#include "Runtime/Public/Component.h"
#include "Runtime/Public/Object.h"

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// 게임 런타임의 OOP Component composition root입니다.
//
// 부착된 Component를 직접 소유하며, GameObject가 파괴되면 부착된
// Component도 함께 파괴됩니다.
//
// Component 접근은 부모가 보관한 포인터 배열을 직접 순회합니다.
// 일상적인 Component 접근에 ObjectHandle 조회를 강제하지 않습니다.
//
// World 소속과 Transform은 GameObject의 기본 책임이 아닙니다.
// ============================================================================

class GameObject : public Object
{
public:
	explicit GameObject(ObjectHandle objectHandle);
	~GameObject() override;

	Component* AddComponent(std::unique_ptr<Component> component);
	bool RemoveComponent(ObjectHandle componentHandle);

	Component* FindComponent(ObjectHandle componentHandle);
	const Component* FindComponent(ObjectHandle componentHandle) const;

	template<typename ComponentType>
	ComponentType* GetComponent();

	template<typename ComponentType>
	const ComponentType* GetComponent() const;

	uint32 GetComponentCount() const;

protected:
	void ClearComponents();

private:
	std::vector<std::unique_ptr<Component>> components;
};

template<typename ComponentType>
ComponentType* GameObject::GetComponent()
{
	const uint32 componentCount = static_cast<uint32>(components.size());

	for (uint32 index = 0; index < componentCount; ++index)
	{
		ComponentType* component = dynamic_cast<ComponentType*>(components[index].get());

		if (component != nullptr)
		{
			return component;
		}
	}

	return nullptr;
}

template<typename ComponentType>
const ComponentType* GameObject::GetComponent() const
{
	const uint32 componentCount = static_cast<uint32>(components.size());

	for (uint32 index = 0; index < componentCount; ++index)
	{
		const ComponentType* component = dynamic_cast<const ComponentType*>(components[index].get());

		if (component != nullptr)
		{
			return component;
		}
	}

	return nullptr;
}
