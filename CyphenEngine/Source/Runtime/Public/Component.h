#pragma once

#include "Runtime/Public/Object.h"
#include "Runtime/Public/UpdateParticipation.h"
#include "Runtime/Public/UpdateFunction.h"

class ObjectManager;
class GameObject;

// ============================================================================
// Component
// ----------------------------------------------------------------------------
// GameObject에 부착되는 OOP 기능 구성요소의 기반입니다.
//
// Component는 부모 없이 먼저 생성할 수 있습니다.
// GameObject에 직접 부착된 이후 composition 구성원으로 취급합니다.
//
// UpdateParticipation은 실행 참여에 대한 선언입니다.
// Component는 UpdateManager에 자신을 직접 등록하거나 해제하지 않습니다.
// 부모 GameObject가 Component의 참여 선언을 확인하고 이를 대행합니다.
//
// CreateUpdateFunction은 요청받은 실행 단계의 임시 UpdateFunction만 생성합니다.
// 실제 등록과 해제 책임은 부모 GameObject에 있습니다.
//
// Component가 부모 GameObject에서 Detach되면 Component 구성 전체는
// 독립적인 실행 구성으로 유지될 수 없습니다.
// 부모 GameObject는 Component와 하위 GameObject의 실행 참여를 제거하고,
// 다시 Attach될 때 중단됐던 실행 참여를 복구합니다.
//
// SetActive 플래그는 현재 실행 그룹 게이트와 연결되어 있지 않습니다.
// 활성 제어는 후속 Manager의 ActivationPivot 정책에서 처리합니다.
// ============================================================================

class Component : public Object
{
public:
	void SetActive(bool activationFlag);
	bool IsActive() const;

	// Lifecycle Functions
	virtual void GlobalUpdate(double deltaSeconds);
	virtual void Update(double deltaSeconds);
	virtual void FinalUpdate(double deltaSeconds);
	virtual void GlobalFinalUpdate(double deltaSeconds);

	bool HasUpdateParticipation(UpdateParticipation participation) const;

	GameObject* GetOwner();
	const GameObject* GetOwner() const;

protected:
	friend class GameObject;

	explicit Component(ObjectHandle objectHandle);
	Component(ObjectHandle objectHandle, UpdateParticipation updateParticipation);

	~Component() override;

	virtual UpdateFunction CreateUpdateFunction(UpdateParticipation participation);

	template<typename TargetType>
	static UpdateFunction MakeUpdateFunction(TargetType& target, UpdateParticipation participation);


	bool CanAttachTo(const Object& outer) const override final;

	bool OnDestroy() override;

private:
	friend class ObjectManager;

	bool isActive = true;

	const UpdateParticipation updateParticipation;
};

template<typename TargetType>
UpdateFunction Component::MakeUpdateFunction(TargetType& target, UpdateParticipation participation)
{
	UpdateExecuteFunction execute = nullptr;

	switch (participation)
	{
	case UpdateParticipation::GlobalUpdate:
		execute = ExecuteUpdateFunction<
			TargetType,
			static_cast<void(TargetType::*)(double)>(&TargetType::GlobalUpdate)>;
		break;

	case UpdateParticipation::Update:
		execute = ExecuteUpdateFunction<
			TargetType,
			static_cast<void(TargetType::*)(double)>(&TargetType::Update)>;
		break;

	case UpdateParticipation::FinalUpdate:
		execute = ExecuteUpdateFunction<
			TargetType,
			static_cast<void(TargetType::*)(double)>(&TargetType::FinalUpdate)>;
		break;

	case UpdateParticipation::GlobalFinalUpdate:
		execute = ExecuteUpdateFunction<
			TargetType,
			static_cast<void(TargetType::*)(double)>(&TargetType::GlobalFinalUpdate)>;
		break;

	default:
		break;
	}

	return
	{
		&target,
		execute
	};
}
