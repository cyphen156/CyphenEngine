#pragma once

#include "Runtime/Public/Object.h"
#include "Runtime/Public/UpdateFunction.h"
#include "Runtime/Public/UpdateParticipation.h"

class GameRuntime;
class ObjectManager;

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// Runtime OOP 객체군을 구성할 수 있는 논리 객체입니다.
//
// GameObject는 GameRuntime::Admit을 통해 Runtime에 소속됩니다.
// GameRuntime 소속은 현재 객체의 runtime 필드에 명시적으로 저장합니다.
// GetGameRuntime은 Outer 관계를 탐색하여 Runtime 소속을 추론하지 않습니다.
//
// Runtime에 참여하는 GameObject 아래에 Subtree가 Attach되면,
// 아직 Runtime이 없는 하위 GameObject는 같은 Runtime에 참여합니다.
// GameObject의 Detach는 기존 Runtime 소속을 제거하지 않습니다.
// 단, Component가 Detach되면 그 아래의 GameObject는 독립 실행 루트가
// 될 수 없으므로 Runtime을 떠납니다.
//
// UpdateParticipation은 실행 참여에 대한 선언입니다.
// 실제 UpdateFunction은 Runtime 등록 또는 해제 시
// 필요한 단계 하나만 생성합니다.
//
// GameObject는 자신과 직접 Component의 UpdateFunction 공급을 책임집니다.
// 직접 Component의 Attach와 Detach도 부모 GameObject가
// 등록과 해제를 대행합니다.
//
// 하위 GameObject는 각자의 Runtime 참여와 실행 문맥을
// 독립적으로 처리합니다.
//
// SetActive 플래그는 현재 실행 그룹 게이트와 연결되어 있지 않습니다.
// 활성 제어는 후속 Manager의 ActivationPivot 정책에서 처리합니다.
// ============================================================================

class GameObject : public Object
{
public:
	bool AttachSubObject(Object& subObject) override;
	bool DetachSubObject(Object& subObject) override;

	void SetActive(bool activationFlag);
	bool IsActive() const;

	virtual void GlobalUpdate(double deltaSeconds);
	virtual void Update(double deltaSeconds);
	virtual void FinalUpdate(double deltaSeconds);
	virtual void GlobalFinalUpdate(double deltaSeconds);

	bool HasUpdateParticipation(UpdateParticipation participation) const;
	bool GetUpdateFunctions(UpdateParticipation participation, std::vector<UpdateFunction>& outUpdateFunctionList);

	uint32 GetComponentCount() const;

	const GameRuntime* GetGameRuntime() const;

protected:
	explicit GameObject(ObjectHandle objectHandle);
	GameObject(ObjectHandle objectHandle, UpdateParticipation updateParticipation);

	~GameObject() override;

	virtual UpdateFunction CreateUpdateFunction(UpdateParticipation updateParticipation);

	template<typename TargetType>
	static UpdateFunction MakeUpdateFunction(TargetType& target, UpdateParticipation participation);
	
	virtual bool IsUpdateExecutionAnchor(UpdateParticipation participation) const;
	bool CanAttachSubtreeTo(const Object& outer) const override;
	
	bool OnDestroy() override;
	void OnAttached() override;
	void OnDetaching() override;

private:
	friend class GameRuntime;
	friend class ObjectManager;

	bool IsRuntimeParticipationPathValid(const GameRuntime& targetRuntime) const;

	bool isActive = true;

	const UpdateParticipation updateParticipation;

	// Runtime에 참여한 모든 GameObject에서 설정합니다.
	GameRuntime* runtime = nullptr;
};

template<typename TargetType>
UpdateFunction GameObject::MakeUpdateFunction(TargetType& target, UpdateParticipation participation)
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
