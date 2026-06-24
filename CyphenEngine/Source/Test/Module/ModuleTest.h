#pragma once

// ============================================================================
// ModuleTests
// ----------------------------------------------------------------------------
// Debug 전용 Module 시스템 테스트 진입점입니다.
//
// 범위:
//   - ModuleCommand / ModuleCommandBuffer의 word stream 계약
//   - ModuleDescriptor / ModuleManager의 최소 검증 계약
//
// 주의:
//   - 실제 DLL 로딩 성공 경로는 테스트용 모듈 Binary가 생긴 뒤 추가합니다.
//   - 이 테스트는 CoreIOTests와 분리합니다.
//     Module 시스템은 Core가 아니라 Source/Modules 계층의 기능입니다.
// ============================================================================
void RunModuleTests();
