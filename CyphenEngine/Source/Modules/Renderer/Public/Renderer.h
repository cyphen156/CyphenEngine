#pragma once

#include "Modules/Renderer/Public/RendererTypes.h"

// ============================================================================
// Renderer
// ----------------------------------------------------------------------------
// Engine이 사용하는 논리 Renderer Module의 공개 Facade입니다.
//
// Renderer는 자신의 논리 Module 정체성으로 선택 구현을 획득하고,
// RendererModuleApi를 조회·검증한 뒤 연결 상태를 보관합니다.
//
// 책임:
//   - 선택 Renderer 구현 Binding
//   - RendererModuleApi 조회 및 버전·타입 검증
//   - Renderer 시스템의 연결 상태
//
// 비책임:
//   - Descriptor 및 Preference 생성
//   - Native Binary 직접 Load/Unload
//   - DX11/Vulkan API 직접 호출
//
// #2_3 범위:
//   - 선택 구현 Binary 획득
//   - RendererModuleApi 바인딩
//   - 역순 참조 해제
//
// 이월:
//   - Render Thread / Command Queue
//   - GPU Device / SwapChain
//   - ExecuteCommandList / Capability
// ============================================================================

namespace Renderer
{
	bool Initialize();
	void Shutdown();

	bool IsInitialized();
	RendererType GetRendererType();
}
