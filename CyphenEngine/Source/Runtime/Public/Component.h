#pragma once

#include "Runtime/Public/Object.h"

class GameObject;
class ObjectManager;

// ============================================================================
// Component
// ----------------------------------------------------------------------------
// GameObject에 부착되는 OOP 기능 구성요소의 기반입니다.
//
// Component는 Object 정체성과 ObjectHandle을 가집니다.
// 동일한 Component 타입이라도 서로 다른 GameObject에 생성된 Component 인스턴스는
// 각각 독립된 ObjectHandle을 가집니다.
//
// Component는 부모 없이 먼저 생성될 수 있습니다.
// 아직 부착되지 않은 Component는 GetOwner가 nullptr을 반환하며,
// GameObject composition의 구성원으로 취급하지 않습니다.
//
// GameObject composition에 부착된 Component의 직접 Outer는 반드시
// 자신이 부착된 GameObject여야 합니다.
// GetOwner는 별도의 Owner 포인터를 저장하지 않고 직접 Outer를 반환합니다.
//
// Component는 GameObject가 관리하는 평면 Component 목록의 구성원입니다.
// Component가 다른 Component의 직접 Outer가 되는 Component in Component 구성은
// 허용하지 않습니다.
//
// Component는 일반 Object 또는 GameObject를 종속 SubObject로 소유할 수 있습니다.
// 이 관계는 Component composition이 아니라 Object의 종속 수명 관계입니다.
// 소유한 GameObject 안에 다시 Component를 부착하면 새로운 composition root가
// 명시적으로 형성됩니다.
//
// Destroy 요청 시 GameObject에 부착되어 있다면 Component 목록과
// Object의 SubObject 관계에서 먼저 분리한 뒤 공통 Object 파괴 경로에 합류합니다.
//
// DOD Storage의 데이터와 Runtime Entity는 Component 계층에 포함하지 않습니다.
// ============================================================================

class Component : public Object
{
public:
	GameObject* GetOwner();
	const GameObject* GetOwner() const;

	bool Destroy() override;

protected:
	explicit Component(ObjectHandle objectHandle);
	~Component() override;

	bool CanAttachTo(const Object& outer) const override final;

private:
	friend class ObjectManager;
	friend class GameObject;
};
