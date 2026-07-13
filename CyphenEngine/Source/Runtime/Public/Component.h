#pragma once

#include "Runtime/Public/Object.h"

// ============================================================================
// Component
// ----------------------------------------------------------------------------
// GameObject에 부착되는 OOP 기능 구성요소의 기반입니다.
//
// Component 자신도 Object 정체성을 가지며, 구체 Component는 고유하거나
// 소수인 Script 행동과 상태를 정의합니다.
//
// DOD Storage의 데이터와 RuntimeEntry는 Component 계층에 포함하지 않습니다.
// ============================================================================

class Component : public Object
{
public:
	~Component() override;

protected:
	explicit Component(ObjectHandle inHandle);
};
