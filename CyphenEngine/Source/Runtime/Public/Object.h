#pragma once

#include "Runtime/Public/Entity.h"

class Object;
class ObjectManager;

using ObjectHandle = EntityHandle<Object>;

// ============================================================================
// Object
// ----------------------------------------------------------------------------
// 엔진의 OOP 객체가 공유하는 정체성과 다형적 수명 기반입니다.
//
// 모든 Object는 정적 ObjectManager가 발급한 유효한 ObjectHandle을 생성 시 주입받으며,
// 생존하는 동안 동일한 ObjectHandle을 통해 식별할 수 있습니다.
// 
// Object는 자신의 Component composition을 대표하는 primary Entity이기도 하며,
// ObjectHandle은 그 primary Entity의 EntityHandle<Object>와 같은 타입입니다.
// Handle 보유 여부는 객체의 실제 소유 위치를 결정하지 않습니다.
//
// 동일한 ObjectHandle을 가진 객체 복제와 주소 기반 소유 관계의 변경을
// 방지하기 위해 복사와 이동을 허용하지 않습니다.
// ============================================================================

class Object
{
public:
	ObjectHandle GetHandle() const;
	void Destroy();

protected:
	explicit Object(ObjectHandle objectHandle);
	virtual ~Object();

private:
	friend class ObjectManager;

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	ObjectHandle handle;
};
