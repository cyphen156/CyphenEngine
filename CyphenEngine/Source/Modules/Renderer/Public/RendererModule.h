#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RendererTypes.h"
#include "Modules/Renderer/Public/RenderCommand.h"

// ============================================================================
// RendererModuleApi
// ----------------------------------------------------------------------------
// Renderer와 선택된 Renderer 구현 DLL 사이의 ABI 계약입니다.
//
// #2_5 추가:
//   - executeCommandList
//
// 주의:
//   - 함수표는 기능마다 늘리지 않습니다.
//   - 렌더링 기능은 RenderCommand IR 데이터로 확장합니다.
// ============================================================================

constexpr uint32 RENDERER_MODULE_ABI_GENERATION = 4;
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

using ExecuteCommandListFunction =
	RendererModuleResult(*)(
		RendererHandle rendererHandle,
		const RenderCommandList* commandList);

struct RendererModuleApi
{
	uint32 apiVersion = 0;
	RendererType rendererType = RendererType::None;

	CreateRendererFunction createRenderer = nullptr;
	DestroyRendererFunction destroyRenderer = nullptr;
	ExecuteCommandListFunction executeCommandList = nullptr;
};

using GetRendererModuleApiFunction =
	RendererModuleResult(*)(RendererModuleApi* outRendererModuleApi);

static_assert(sizeof(void*) == 8, "RendererModuleApi currently assumes x64.");
static_assert(sizeof(RendererModuleApi) == 32, "RendererModuleApi must be 32 bytes on x64.");
