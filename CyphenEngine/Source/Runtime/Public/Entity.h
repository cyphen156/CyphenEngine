#pragma once

#include "Core/Public/Handle.h"

// ============================================================================
// Entity
// ----------------------------------------------------------------------------
// DOD Component composition의 정체성을 참조하는 typed Handle입니다.
//
// Entity는 대응하는 OOP Object 없이 독립적으로 존재할 수 있습니다.
// Object가 Entity composition에 참여하는 경우 두 정체성은 명시적으로 연결합니다.
//
// ObjectHandle과 EntityHandle은 서로 다른 관리 경계가 발급하고 검증합니다.
// 두 Handle의 내부 값이 같더라도 같은 대상을 의미하지 않습니다.
//
// 서로 다른 Handle 도메인을 공통 정체성으로 연결해야 하는 경우에는
// Handle 값을 공유하지 않고 별도의 UID를 사용합니다.
// ============================================================================

template<typename CompositionType>
struct EntityHandleTag
{
};

template<typename CompositionType>
using EntityHandle = Handle<EntityHandleTag<CompositionType>>;
