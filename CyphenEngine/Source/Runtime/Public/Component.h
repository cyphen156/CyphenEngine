#pragma once

#include "Runtime/Public/Object.h"

class GameObject;
class ObjectManager;

// ============================================================================
// Component
// ----------------------------------------------------------------------------
// GameObject에 부착되는 OOP 기능 구성요소의 기반입니다.
//
// Component는 Object 정체성과 ObjectHandle을 유지합니다.
// 부착 관계는 GameObject composition에 귀속되며, 실제 메모리 수명과 UID는
// 엔진 공통 ObjectManager가 관리합니다.
//
// DOD Storage의 데이터와 RuntimeEntry는 Component 계층에 포함하지 않습니다.
// ============================================================================

class Component : public Object
{
public:
	GameObject* GetOwner();
	const GameObject* GetOwner() const;

protected:
	Component(ObjectHandle objectHandle, GameObject& owner);
	~Component() override;

private:
	friend class ObjectManager;
	friend class GameObject;

	GameObject* owner;
};
