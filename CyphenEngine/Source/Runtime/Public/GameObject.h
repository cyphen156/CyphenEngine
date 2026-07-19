#pragma once

#include <utility>
#include <vector>

#include "Runtime/Public/Component.h"
#include "Runtime/Public/Object.h"
#include "Runtime/Public/ObjectManager.h"

class ObjectManager;

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// 게임 실행을 위한 OOP composition root입니다.
//
// 부착된 Component는 GameObject의 직접 SubObject로 연결합니다.
// 모든 Component는 하나의 GameObject에 직접 부착된 평면 목록의 구성원이며,
// Component가 다른 Component를 직접 소유하는 composition 계층은 허용하지 않습니다.
//
// GameObject가 파괴되면 ObjectManager의 종속 수명 순회를 통해 Component에도
// 파괴가 요청되며, 실제 메모리 수명은 ObjectManager가 관리합니다.
//
// components는 Component 타입에 특화된 평면 조회 인덱스입니다.
// 종속 수명의 정본은 Object의 Outer/SubObject 관계입니다.
//
// GameObject는 일반 Object, 다른 GameObject와 Component를 SubObject로
// 소유할 수 있습니다.
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

private:
	friend class Component;
	friend class ObjectManager;

	bool DetachComponent(Component* component);

	std::vector<Component*> components;
};

template<typename ComponentType, typename... ArgumentTypes>
ComponentType* GameObject::AddComponent(ArgumentTypes&&... arguments)
{
	ComponentType* component = Object::NewObject<ComponentType>(
		std::forward<ArgumentTypes>(arguments)...);

	if (component == nullptr)
	{
		return nullptr;
	}

	if (AttachSubObject(*component) == false)
	{
		component->Object::Destroy();

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
		const ComponentType* component = dynamic_cast<const ComponentType*>(components[index]);

		if (component != nullptr)
		{
			return component;
		}
	}

	return nullptr;
}
