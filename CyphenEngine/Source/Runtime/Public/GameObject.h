#pragma once

#include "Runtime/Public/Object.h"
#include "Runtime/Public/UpdateParticipation.h"

class GameRuntime;
class ObjectManager;

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// Runtime OOP 객체군을 구성할 수 있는 논리 객체입니다.
//
// 루트 GameObject는 GameRuntime::Admit을 통해 Runtime에 소속됩니다.
// GameRuntime 소속은 현재 객체의 runtime 필드에 명시적으로 저장합니다.
// GetGameRuntime은 Outer 관계를 탐색하여 Runtime 소속을 추론하지 않습니다.
//
// 하위 GameObject의 Runtime 소속 변경은 관계가 변경되는
// Attach / Detach 처리에서 수행합니다.
//
// UpdateParticipation은 실행 능력의 선언입니다.
// 실제 등록은 Runtime 또는 World의 명시적인 소속 처리에서 수행합니다.
// ============================================================================

class GameObject : public Object
{
public:
	void SetActive(bool activationFlag);
	bool IsActive() const;

	bool Destroy() override;

	virtual void GlobalUpdate(double deltaSeconds);
	virtual void Update(double deltaSeconds);
	virtual void FinalUpdate(double deltaSeconds);
	virtual void GlobalFinalUpdate(double deltaSeconds);

	bool HasUpdateParticipation(UpdateParticipation participation) const;

	uint32 GetComponentCount() const;

	const GameRuntime* GetGameRuntime() const;

protected:
	explicit GameObject(ObjectHandle objectHandle);
	GameObject(ObjectHandle objectHandle, UpdateParticipation updateParticipation);

	~GameObject() override;

private:
	friend class GameRuntime;
	friend class ObjectManager;

	bool isActive = true;

	const UpdateParticipation updateParticipation;

	// 직접 Admit된 루트 GameObject에서만 설정합니다.
	GameRuntime* runtime = nullptr;
};
