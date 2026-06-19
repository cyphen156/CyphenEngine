#pragma once

#include "Core/Public/CPrimitiveTypes.h"

enum class RendererType : uint32
{
	Dx11,
	Dx12,
	Vulkan,
	OpenGL,
	Metal,

	None
};
