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

bool GameObject::CanAttachSubtreeTo(const Object& outer) const
{
	const GameRuntime* outerRuntime = nullptr;
	const Object* ancestor = &outer;

	while (ancestor != nullptr)
	{
		const GameObject* gameObject = dynamic_cast<const GameObject*>(ancestor);

		if (gameObject == nullptr)
		{
			ancestor = ancestor->GetOuter();
			continue;
		}

		outerRuntime = gameObject->runtime;
		break;
	}

	if (runtime != nullptr && outerRuntime != nullptr && runtime != outerRuntime)
	{
		return false;
	}

	return Object::CanAttachSubtreeTo(runtime != nullptr ? *this : outer);
}

void GameObject::OnAttached()
{
	// 아직 런타임에 참여하지 않은 경우에만
	// 가장 가까운 상위 GameObject의 런타임 참여 상태를 확인한다.
	if (runtime == nullptr)
	{
		Object* ancestor = GetOuter();

		while (ancestor != nullptr)
		{
			GameObject* gameObject = dynamic_cast<GameObject*>(ancestor);

			// GameObject가 아니라면 런타임 경계가 아니므로 상위 객체를 계속 확인한다.
			if (gameObject == nullptr)
			{
				ancestor = ancestor->GetOuter();
				continue;
			}

			// 가장 가까운 상위 GameObject가 런타임에 참여 중이면
			// 자신도 같은 런타임에 참여한다.
			if (gameObject->runtime != nullptr)
			{
				gameObject->runtime->Admit(*this);
			}

			// 가장 가까운 GameObject만 소속 기준으로 사용한다.
			// 해당 GameObject의 runtime이 nullptr이어도 더 위는 탐색하지 않는다.
			break;
		}
	}
	Object::OnAttached();
}
