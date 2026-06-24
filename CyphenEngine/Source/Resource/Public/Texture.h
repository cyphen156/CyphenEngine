#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Resource/Public/Resource.h"

// ============================================================================
// Texture
// ----------------------------------------------------------------------------
// Texture 계열 Resource 자료형입니다.
//
// Texture2D:
//   - CPU-side decoded texture data입니다.
//   - GPU resource가 아닙니다.
//   - Backend에 ResourceCommand로 전달되기 전의 Resource 표현입니다.
//
// 비책임:
//   - JPEG / PNG 등 파일 포맷 decode
//   - Backend texture object / SRV / descriptor 관리
//   - Draw command 생성
// ============================================================================

enum class TextureFormat : uint32
{
	Unknown = 0,
	Rgba8 = 1
};

struct Texture2D : public Resource
{
	Texture2D();

	TextureFormat format = TextureFormat::Unknown;
	std::vector<uint8> pixels;
	uint32 width = 0;
	uint32 height = 0;
};
