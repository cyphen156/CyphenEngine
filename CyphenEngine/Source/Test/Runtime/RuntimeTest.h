#pragma once

#include <vector>

#include "Resource/Public/Resource.h"

class GameRuntime;
class World;

// ============================================================================
// RuntimeTest
// ----------------------------------------------------------------------------
// Resource 업로드 이후 Runtime 객체의 생성, 구성과 World 합류 계약을 확인합니다.
//
// Square WorldObject 두 개를 생성하고, 서로 다른 Texture ResourceId를 사용하는
// Sprite Component를 각 Square의 직접 SubObject로 부착합니다.
//
// ObjectManager 등록, Outer/SubObject 관계, Component Owner와 World Join 결과,
// Runtime/World 중복 합류 거부와 종속 파괴 경로를 항목별로 검증합니다.
//
// 파괴 검증이 끝나면 같은 구성의 Square와 Sprite 두 쌍을 다시 생성하고
// Runtime과 World에 유지합니다. 유지된 객체 구성은 이후 World 상태에서
// Render Queue에 전달할 Frame 정보를 추출하는 경로의 타깃 fixture로 사용합니다.
//
// Renderer Resource 업로드 자체와 Frame 생성은 현재 테스트의 범위가 아닙니다.
// ============================================================================

void RunRuntimeTests(GameRuntime& runtime, World& world, const std::vector<ResourceId>& resourceIds);
