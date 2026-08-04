#include "pch.h"

#include "Runtime/Public/GameObject.h"
#include "Runtime/Public/Component.h"
#include "Runtime/Public/GameRuntime.h"

bool GameObject::AttachSubObject(Object& subObject)
{
	// 이미 현재 GameObject의 직접 SubObject라면
	// 실행 참여를 다시 등록하지 않습니다.
	if (subObject.GetOuter() == this)
	{
		return true;
	}

	Component* component = dynamic_cast<Component*>(&subObject);

	// 구조 검사, 관계 확정과 OnAttached 전파는
	// 공통 Object 계약이 먼저 처리합니다.
	//
	// 관계가 확정된 이후 실행 등록이 실패하더라도
	// 이미 확정된 Object 관계를 원복하지 않습니다.
	if (Object::AttachSubObject(subObject) == false)
	{
		return false;
	}

	// GameObject와 일반 Object의 하위 GameObject 참여는
	// OnAttached 재귀 전파에서 스스로 처리합니다.
	//
	// 직접 부모가 추가로 대행해야 하는 대상은
	// 직접 부착된 Component 하나뿐입니다.
	if (component == nullptr || runtime == nullptr)
	{
		return true;
	}

	constexpr UpdateParticipation updateParticipations[] =
	{
		UpdateParticipation::GlobalUpdate,
		UpdateParticipation::Update,
		UpdateParticipation::FinalUpdate,
		UpdateParticipation::GlobalFinalUpdate
	};

	std::vector<UpdateFunction> updateFunctionList;
	updateFunctionList.reserve(1);

	// Component는 GameRuntime을 알지 않습니다.
	// 직접 부모 GameObject가 Component 하나의 실행 참여를 대행합니다.
	for (UpdateParticipation participation : updateParticipations)
	{
		if (component->HasUpdateParticipation(participation) == false)
		{
			continue;
		}

		updateFunctionList.clear();
		updateFunctionList.push_back(component->CreateUpdateFunction(participation));

		// 관계가 확정된 이후의 실행 등록 실패는
		// Attach 관계를 원복하지 않습니다.
		//
		// 호출자에게 부분 실패를 알리기 위해 false를 반환하지만,
		// Component는 이미 현재 GameObject의 SubObject입니다.
		if (runtime->RegisterUpdateFunction(*this, participation, updateFunctionList) == false)
		{
#if _DEBUG
			PRINT_DEBUG_OUTPUT("[GameObject] Attach 이후 Component의 UpdateFunction을 등록하지 못했습니다.\n");
#endif
			return false;
		}
	}

	return true;
}

bool GameObject::DetachSubObject(Object& subObject)
{
	// 실행그룹을 먼저 변경하므로
	// 실제 직접 자식 관계인지 먼저 확인합니다.
	if (subObject.GetOuter() != this)
	{
		return false;
	}

	Component* component = dynamic_cast<Component*>(&subObject);

	if (component != nullptr && runtime != nullptr)
	{
		constexpr UpdateParticipation updateParticipations[] =
		{
			UpdateParticipation::GlobalUpdate,
			UpdateParticipation::Update,
			UpdateParticipation::FinalUpdate,
			UpdateParticipation::GlobalFinalUpdate
		};

		std::vector<UpdateFunction> updateFunctionList;
		updateFunctionList.reserve(1);

		// Component 자신의 실행 참여는
		// 직접 부모 GameObject가 해제합니다.
		for (UpdateParticipation participation : updateParticipations)
		{
			if (component->HasUpdateParticipation(participation) == false)
			{
				continue;
			}

			updateFunctionList.clear();
			updateFunctionList.push_back(component->CreateUpdateFunction(participation));

			// Detach는 실행 장부 불일치로 거부하지 않습니다.
			// 실패한 단계만 보고하고 다음 정리를 계속합니다.
			if (runtime->UnregisterUpdateFunction(*this, participation, updateFunctionList) == false)
			{
#if _DEBUG
				PRINT_DEBUG_OUTPUT("[GameObject] Detach 중 Component의 UpdateFunction을 해제하지 못했습니다.\n");
#endif
			}
		}

		std::vector<Object*> pendingObjects;
		pendingObjects.push_back(component);

		// Component는 독립적인 Runtime 구성 루트가 될 수 없습니다.
		//
		// Component 아래에서 각 분기마다 처음 발견되는 GameObject를
		// Runtime Leave의 루트로 사용합니다.
		//
		// GameRuntime::Leave가 해당 GameObject의 전체 Subtree를
		// 처리하므로 GameObject를 발견한 분기는 더 탐색하지 않습니다.
		while (pendingObjects.empty() == false)
		{
			Object* object = pendingObjects.back();
			pendingObjects.pop_back();

			GameObject* childGameObject = dynamic_cast<GameObject*>(object);

			if (childGameObject != nullptr)
			{
				if (childGameObject->runtime == runtime && runtime->Leave( *childGameObject) == false)
				{
#if _DEBUG
					PRINT_DEBUG_OUTPUT("[GameObject] Component Detach 중 하위 GameObject가 Runtime을 떠나지 못했습니다.\n");
#endif
				}

				continue;
			}

			const uint32 subObjectCount = object->GetSubObjectCount();

			for (uint32 index = 0; index < subObjectCount; ++index)
			{
				Object* childObject = object->GetSubObject(subObjectCount - index - 1);

				if (childObject != nullptr)
				{
					pendingObjects.push_back(childObject);
				}
			}
		}
	}

	// 유효한 Detach는 실행 정리 실패로 거부하지 않습니다.
	//
	// Component 정리는 위에서 부모가 처리했고,
	// 일반 Object와 GameObject의 World-local 정리는
	// OnDetaching 전파가 처리합니다.
	return Object::DetachSubObject(subObject);
}

void GameObject::SetActive(bool activationFlag)
{
	isActive = activationFlag;
}

bool GameObject::IsActive() const
{
	return isActive;
}

void GameObject::GlobalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);
}

void GameObject::Update(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);
}

void GameObject::FinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);
}

void GameObject::GlobalFinalUpdate(double deltaSeconds)
{
	static_cast<void>(deltaSeconds);
}

bool GameObject::HasUpdateParticipation(UpdateParticipation participation) const
{
	return (static_cast<uint8>(updateParticipation) & static_cast<uint8>(participation)) != 0;
}

bool GameObject::GetUpdateFunctions(UpdateParticipation participation, std::vector<UpdateFunction>& outUpdateFunctionList)
{
	outUpdateFunctionList.clear();

	// GameObject 자신이 요청받은 단계에 참여한다면
	// 해당 단계의 임시 UpdateFunction을 목록에 추가합니다.
	if (HasUpdateParticipation(participation))
	{
		const UpdateFunction updateFunction = CreateUpdateFunction(participation);

		if (updateFunction.target == nullptr || 
			updateFunction.execute == nullptr)
		{
			return false;
		}

		outUpdateFunctionList.push_back(updateFunction);
	}

	const uint32 subObjectCount = GetSubObjectCount();

	for (uint32 index = 0; index < subObjectCount; ++index)
	{
		Object* subObject = GetSubObject(index);

		if (subObject == nullptr)
		{
			outUpdateFunctionList.clear();

			return false;
		}

		// GameObject는 직접 부착된 Component의 실행 참여를 대리합니다.
		// 일반 Object와 자식 GameObject는 여기서 처리하지 않습니다.
		Component* component = dynamic_cast<Component*>(subObject);

		if (component == nullptr || component->HasUpdateParticipation(participation) == false)
		{
			continue;
		}

		const UpdateFunction updateFunction = component->CreateUpdateFunction(participation);

		if (updateFunction.target == nullptr ||
			updateFunction.execute == nullptr)
		{
			outUpdateFunctionList.clear();

			return false;
		}

		outUpdateFunctionList.push_back(updateFunction);
	}

	return true;
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

UpdateFunction GameObject::CreateUpdateFunction(UpdateParticipation participation)
{
	return MakeUpdateFunction(*this, participation);
}


bool GameObject::IsUpdateExecutionAnchor(UpdateParticipation participation) const
{
	static_cast<void>(participation);

	return false;
}

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

bool GameObject::OnDestroy()
{
	if (runtime != nullptr && runtime->Leave(*this) == false)
	{
		return false;
	}

	return Object::OnDestroy();
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

void GameObject::OnDetaching()
{
	// WorldObject가 현재 World 실행 앵커라면
	// 구조적 Detach만으로 World 참여를 제거하지 않습니다.
	//
	// 해당 WorldObject와 하위 실행 범위는 기존 World 문맥을 유지하므로
	// 이 지점에서 재귀 전파도 중단합니다.
	if (IsUpdateExecutionAnchor(UpdateParticipation::Update) || IsUpdateExecutionAnchor(UpdateParticipation::FinalUpdate))
	{
		return;
	}

	if (runtime != nullptr)
	{
		constexpr UpdateParticipation worldUpdateParticipations[] =
		{
			UpdateParticipation::Update,
			UpdateParticipation::FinalUpdate
		};

		std::vector<UpdateFunction> updateFunctionList;

		// 일반 GameObject는 상위 관계에서 분리되면
		// 물려받았던 World-local 실행 문맥을 잃습니다.
		//
		// Runtime 소속과 GU/GFU는 그대로 유지합니다.
		for (UpdateParticipation participation : worldUpdateParticipations)
		{
			if (GetUpdateFunctions(participation, updateFunctionList) == false)
			{
#if _DEBUG
				PRINT_DEBUG_OUTPUT("[GameObject] Detach 중 World-local UpdateFunction 목록을 만들지 못했습니다.\n");
#endif
				continue;
			}

			if (runtime->UnregisterUpdateFunction(*this, participation, updateFunctionList) == false)
			{
#if _DEBUG
				PRINT_DEBUG_OUTPUT("[GameObject] Detach 중 World-local UpdateFunction을 해제하지 못했습니다.\n");
#endif
			}
		}
	}

	// 일반 Object는 실행 문맥이 없으므로 통과하고,
	// 일반 GameObject는 같은 규칙을 자기 Subtree에 전파합니다.
	//
	// 하위 WorldObject를 만나면 해당 객체의
	// IsUpdateExecutionAnchor 검사에서 가지치기됩니다.
	Object::OnDetaching();
}

bool GameObject::IsRuntimeParticipationPathValid(const GameRuntime& targetRuntime) const
{
	const Object* ancestor = GetOuter();
	bool componentBoundaryPending = false;

	while (ancestor != nullptr)
	{
		const Component* component = dynamic_cast<const Component*>(ancestor);

		if (component != nullptr)
		{
			// Component 아래의 GameObject는
			// 독립적인 Runtime 참여 루트가 될 수 없습니다.
			//
			// 위에서 현재 Runtime에 참여하는
			// 직접 composition 소유 GameObject가 확인되어야 합니다.
			componentBoundaryPending = true;
			ancestor = ancestor->GetOuter();
			continue;
		}

		const GameObject* gameObject = dynamic_cast<const GameObject*>(ancestor);

		if (gameObject != nullptr && componentBoundaryPending)
		{
			return gameObject->runtime == &targetRuntime;
		}

		ancestor = ancestor->GetOuter();
	}

	// Component 경계를 통과했지만
	// 이를 소유하는 Runtime 참여 GameObject를 찾지 못했다면
	// 현재 GameObject는 독립적으로 Admit될 수 없습니다.
	return componentBoundaryPending == false;
}
