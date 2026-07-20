#pragma once

#include "Resource/Public/Resource.h"

class GameRuntime;

// ============================================================================
// RuntimeTest
// ----------------------------------------------------------------------------
// Resource 업로드 이후 Runtime 객체의 생성, 구성과 World 합류 계약을 확인합니다.
//
// Square WorldObject를 생성하고, 업로드된 Texture ResourceId를 사용하는
// Sprite Component를 직접 SubObject로 부착합니다.
//
// ObjectManager 등록, Outer/SubObject 관계, Component Owner와 World Join 결과를
// 확인한 뒤 성공한 객체를 Runtime에 유지합니다.
//
// Renderer Resource 업로드 자체와 Frame 생성은 이 테스트의 범위가 아닙니다.
// ============================================================================

bool RunRuntimeTest(GameRuntime& runtime, ResourceId textureId);
