#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "Modules/Renderer/Public/RendererTypes.h"

constexpr uint32 RENDERER_MODULE_API_VERSION = 1;
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
