#pragma once

#include "Modules/Renderer/Public/RendererTypes.h"

// ============================================================================
// Renderer
// ----------------------------------------------------------------------------
// 엔진의 고수준 Renderer 접근 API입니다.
//
// Renderer는 ModuleManager에 이미 로드된 모듈 중 Renderer 계약을 구현한
// 모듈을 찾아 연결합니다.
//
// 책임:
//   - Renderer 모듈 API 조회
//   - Renderer API 버전 및 타입 검증
//   - 활성 Renderer 상태 관리
//
// 비책임:
//   - 동적 라이브러리 로드 / 언로드
//   - 모듈 Enabled 상태 관리
//   - 플랫폼 그래픽 API 직접 호출
//
// Renderer Module의 로드와 수명은 ModuleManager가 담당합니다.
// ============================================================================

namespace Renderer
{
	bool Initialize();
	void Shutdown();

	bool IsInitialized();
	RendererType GetRendererType();
}
