#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RendererTypes.h"

// ============================================================================
// RendererModuleApi
// ----------------------------------------------------------------------------
// Renderer와 선택 구현 DLL 사이의 ABI 계약입니다.
//
// #2_4 책임:
//   - Renderer 실행 인스턴스 생성
//   - Renderer 실행 인스턴스 파괴
//
// 이월:
//   - ExecuteCommandList
//   - RendererCapabilities
//
// ABI 구조가 바뀌면 RENDERER_MODULE_ABI_GENERATION을 증가시킵니다.
// 출시 버전과 ABI 세대 번호는 별개입니다.
// ============================================================================

constexpr uint32 RENDERER_MODULE_ABI_GENERATION = 3;
constexpr uint32 RENDERER_MODULE_API_VERSION = RENDERER_MODULE_ABI_GENERATION;

constexpr const char GET_RENDERER_MODULE_API_NAME[] = "GetRendererModuleApi";

enum class RendererModuleResult : uint32
{
	Failure = 0,
	Success = 1
};

using CreateRendererFunction =
	RendererModuleResult(*)(
		const NativeWindowInfo* windowInfo,
		RendererHandle* outRendererHandle);

using DestroyRendererFunction =
	void(*)(RendererHandle rendererHandle);

struct RendererModuleApi
{
	uint32 apiVersion = 0;
	RendererType rendererType = RendererType::None;

	CreateRendererFunction createRenderer = nullptr;
	DestroyRendererFunction destroyRenderer = nullptr;
};

using GetRendererModuleApiFunction =
	RendererModuleResult(*)(RendererModuleApi* outRendererModuleApi);
