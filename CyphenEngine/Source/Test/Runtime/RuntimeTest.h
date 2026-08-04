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
// 전달받는 world는 엔진이 만든 첫 번째 World입니다.
// 나머지 World는 GameRuntime의 World 조회 접근자로 확인하며,
// 서브트리 경로 정합성 검증도 같은 실환경을 대상으로 수행합니다.
//
// 파괴 검증이 끝나면 같은 구성의 Square와 Sprite 두 쌍을 다시 생성하고
// 최종 유지 상태를 다음 기준으로 구성합니다.
//   - Runtime 1, World 3
//   - Total Object 10000
//   - GameObject 5000 (WorldObject 2500, Square 두 개 포함)
//   - Component 2500 (Sprite 두 개 포함), 일반 Object 2500
//   - WorldObject는 세 World에 순환 배치합니다.
//   - 16가지 UpdateParticipation 조합의 테스트 타입을 모두 부착합니다.
//
// 유지된 객체 구성은 이후 World 상태에서 Render Queue에 전달할 Frame 정보를
// 추출하는 경로와 Scheduler 검증의 타깃 fixture로 사용합니다.
//
// Renderer Resource 업로드 자체와 Frame 생성은 현재 테스트의 범위가 아닙니다.
// ============================================================================

void RunRuntimeTests(GameRuntime& runtime, World& world, const std::vector<ResourceId>& resourceIds);

// RunRuntimeTests가 구성한 Update 실행 대상을 첫 Runtime Tick 이후 검증합니다.
// 각 구체 테스트 타입이 선언한 UpdateParticipation 단계만 정확히 한 번
// 실행되었는지 확인하고 Runtime 테스트 결과를 최종 출력합니다.
void VerifyRuntimeUpdateTests();
