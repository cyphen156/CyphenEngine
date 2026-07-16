#pragma once

#include <utility>
#include <vector>

#include "Runtime/Public/Component.h"
#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

class ObjectManager;
class World;

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// 게임 런타임의 OOP Component composition root입니다.
//
// 부착된 Component의 composition을 소유하며, GameObject가 파괴되면 부착된
// Component에도 파괴를 요청합니다. 실제 메모리 수명은 ObjectManager가 관리합니다.
//
// Component 접근은 부모가 보관한 포인터 배열을 직접 순회합니다.
// 일상적인 Component 접근에 ObjectHandle 조회를 강제하지 않습니다.
//
// Update는 실행 도메인의 System 처리 전에 수행하는 일반 행위입니다.
// FinalUpdate는 System 처리로 확정된 결과를 소비하는 후행 행위입니다.
// 실제 호출 여부와 global / World-local 실행 범위는 등록된 실행 도메인이 결정합니다.
//
// World 소속과 Transform은 GameObject의 기본 책임이 아닙니다.
// ============================================================================

class GameObject : public Object
{
public:
	template<typename ComponentType, typename... ArgumentTypes>
	ComponentType* AddComponent(ArgumentTypes&&... arguments);

	Component* FindComponent(ObjectHandle componentHandle);
	const Component* FindComponent(ObjectHandle componentHandle) const;

	template<typename ComponentType>
	ComponentType* GetComponent();

	template<typename ComponentType>
	const ComponentType* GetComponent() const;

	uint32 GetComponentCount() const;

protected:
	explicit GameObject(ObjectHandle objectHandle);
	~GameObject() override;

	virtual void Update(double deltaSeconds);
	virtual void FinalUpdate(double deltaSeconds);

	void ClearComponents();

private:
	friend class Component;
	friend class ObjectManager;
	friend class World;

	void DetachComponent(Component* component);

	std::vector<Component*> components;
};

template<typename ComponentType, typename... ArgumentTypes>
ComponentType* GameObject::AddComponent(ArgumentTypes&&... arguments)
{
	ComponentType* component = Object::NewObject<ComponentType>(
		*this,
		std::forward<ArgumentTypes>(arguments)...);

	if (component == nullptr)
	{
		return nullptr;
	}

	components.push_back(component);

	return component;
}

template<typename ComponentType>
ComponentType* GameObject::GetComponent()
{
	const uint32 componentCount = static_cast<uint32>(components.size());

	for (uint32 index = 0; index < componentCount; ++index)
	{
		ComponentType* component = dynamic_cast<ComponentType*>(components[index]);

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
		const ComponentType* component =
			dynamic_cast<const ComponentType*>(components[index]);

		if (component != nullptr)
		{
			return component;
		}
	}

	return nullptr;
}
