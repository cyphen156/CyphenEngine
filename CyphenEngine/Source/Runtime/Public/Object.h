#pragma once

#include "Core/Public/Handle.h"

class Object;
using ObjectHandle = Handle<Object>;

// ============================================================================
// Object
// ----------------------------------------------------------------------------
// 엔진의 OOP 객체가 공유하는 정체성과 다형적 수명 기반입니다.
//
// ObjectHandle을 소유하며, 파생 객체를 Object 경계에서 참조하고 제거할 수
// 있도록 공통 수명 계약을 제공합니다.
//
// Component 구성, World 소속, Transform, Tick 정책은 Object의 책임이 아닙니다.
// DOD 객체는 Object를 상속하지 않습니다.
// ============================================================================

class Object
{
public:
	virtual ~Object();

	ObjectHandle GetHandle() const;

protected:
	explicit Object(ObjectHandle inHandle);

private:
	ObjectHandle handle;
};
