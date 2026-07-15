#pragma once

#include "Core/Public/Handle.h"

// ============================================================================
// Entity
// ----------------------------------------------------------------------------
// DOD Component composition의 논리적 정체성을 참조하는 typed UID입니다.
//
// Entity를 무명의 하나의 타입으로 소거하지 않고 composition 타입을 그대로 보존합니다. 
//
// Object는 자신의 Component composition을 대표하는 primary Entity이기도 합니다.
// ObjectHandle은 EntityHandle<Object>이며, 두 관점은 별도 UID나 변환 매핑을
// 필요로 하지 않습니다.
// ============================================================================

template<typename CompositionType>
using EntityHandle = Handle<CompositionType>;
