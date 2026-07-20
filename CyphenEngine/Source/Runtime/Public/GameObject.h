#pragma once

#include "Runtime/Public/Object.h"
#include "Runtime/Public/UpdateParticipation.h"

class ObjectManager;

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// 게임 실행을 위한 OOP composition root입니다.
//
// Component는 GameObject의 직접 SubObject로 부착합니다.
// 모든 Component는 하나의 GameObject에 직접 부착된 평면 구성원이며,
// Component가 다른 Component를 직접 소유하는 composition 계층은 허용하지 않습니다.
//
// 새로운 Component의 생성과 부착은 Object::AddSubObject를 사용합니다.
// 이미 생성된 Component의 부착과 분리는 Object::AttachSubObject와
// Object::DetachSubObject를 사용합니다.
//
// GameObject가 파괴되면 ObjectManager의 종속 수명 순회를 통해 Component에도
// 파괴가 요청되며, 실제 메모리 수명은 ObjectManager가 관리합니다.
//
// Component 관계의 정본은 Object의 Outer/SubObject 관계입니다.
// GetComponentCount는 직접 SubObject 중 Component 구성원의 수를 반환합니다.
//
// GameObject는 일반 Object, 다른 GameObject와 Component를 SubObject로
// 소유할 수 있습니다.
//
// World 소속과 Transform은 GameObject의 기본 책임이 아닙니다.
// ============================================================================

class GameObject : public Object
{
public:
	// Lifecycle Functions
	virtual void GlobalUpdate(double deltaSeconds);
	virtual void Update(double deltaSeconds);
	virtual void FinalUpdate(double deltaSeconds);
	virtual void GlobalFinalUpdate(double deltaSeconds);

	bool HasUpdateParticipation(UpdateParticipation participation) const;

	uint32 GetComponentCount() const;

protected:
	explicit GameObject(ObjectHandle objectHandle);
	GameObject(ObjectHandle objectHandle, UpdateParticipation updateParticipation);

	~GameObject() override;

private:
	friend class ObjectManager;

	const UpdateParticipation updateParticipation;
};
