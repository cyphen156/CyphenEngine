#pragma once

#include "Runtime/Public/Object.h"

// ============================================================================
// GameObject
// ----------------------------------------------------------------------------
// 게임 런타임의 OOP 객체 구성 단위입니다.
//
// Component를 부착할 수 있는 composition root이며, Object의 정체성과
// 다형적 수명 계약을 따릅니다.
//
// World 소속과 Transform은 GameObject에 기본으로 강제하지 않습니다.
// ============================================================================

class GameObject : public Object
{
public:
	explicit GameObject(ObjectHandle inHandle);
	~GameObject() override;
};
