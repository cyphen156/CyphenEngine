#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "Modules/Renderer/Public/RendererTypes.h"

#ifdef _DEBUG
constexpr uint32 RENDERER_MODULE_DEVELOPMENT_VERSION = 2;
constexpr uint32 RENDERER_MODULE_API_VERSION = RENDERER_MODULE_DEVELOPMENT_VERSION;
#else
constexpr uint32 RENDERER_MODULE_API_VERSION = 1;
#endif

constexpr const char GET_RENDERER_MODULE_API_NAME[] = "GetRendererModuleApi";

enum class RendererModuleResult : uint32
{
	Failure = 0,
	Success = 1
};

struct RendererModuleApi
{
	uint32 apiVersion = 0;
	RendererType rendererType = RendererType::None;
};

using GetRendererModuleApiFunction =
RendererModuleResult(*)(RendererModuleApi* outRendererModuleApi);
