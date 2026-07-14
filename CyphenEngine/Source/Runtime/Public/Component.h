#pragma once

#include "Runtime/Public/Object.h"

class GameObject;

// ============================================================================
// Component
// ----------------------------------------------------------------------------
// GameObject에 부착되는 OOP 기능 구성요소의 기반입니다.
//
// Component는 Object 정체성과 ObjectHandle을 유지합니다.
// 다만 부착된 Component의 실제 소유와 수명은 GameObject에 귀속됩니다.
//
// DOD Storage의 데이터와 RuntimeEntry는 Component 계층에 포함하지 않습니다.
// ============================================================================

class Component : public Object
{
public:
	~Component() override;

	GameObject* GetOwner();
	const GameObject* GetOwner() const;

protected:
	explicit Component(ObjectHandle objectHandle);

private:
	friend class GameObject;

	void BindOwner(GameObject* newOwner);

	GameObject* owner = nullptr;
};
