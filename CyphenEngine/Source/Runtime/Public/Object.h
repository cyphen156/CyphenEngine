#pragma once

#include "Core/Public/Handle.h"

class Object;
using ObjectHandle = Handle<Object>;

// ============================================================================
// Object
// ----------------------------------------------------------------------------
// 엔진의 OOP 객체가 공유하는 정체성과 다형적 수명 기반입니다.
//
// 모든 Object는 ObjectHandle을 통해 식별할 수 있습니다.
// Handle 보유 여부는 객체의 실제 소유 위치를 결정하지 않습니다.
//
// 동일한 ObjectHandle을 가진 객체 복제를 방지하기 위해 복사와 이동을
// 허용하지 않습니다.
// ============================================================================

class Object
{
public:
	virtual ~Object();

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	ObjectHandle GetHandle() const;

protected:
	explicit Object(ObjectHandle objectHandle);

private:
	ObjectHandle handle;
};
