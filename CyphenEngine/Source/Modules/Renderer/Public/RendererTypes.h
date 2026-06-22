#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// RendererTypes
// ----------------------------------------------------------------------------
// Renderer와 구현 DLL이 공유하는 Renderer 도메인 기본 타입입니다.
// ============================================================================

enum class RendererType : uint32
{
	Dx11,
	Dx12,
	Vulkan,
	OpenGL,
	Metal,

	None
};

// Renderer 구현 DLL 내부 실행 인스턴스를 가리키는 Opaque Handle입니다.
// 실제 객체 타입과 메모리 수명은 구현 DLL이 소유합니다.
using RendererHandle = void*;
