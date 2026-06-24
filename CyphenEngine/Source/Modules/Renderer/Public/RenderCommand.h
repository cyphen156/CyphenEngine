#pragma once

#include "Modules/Public/Command.h"
#include "Modules/Public/CommandBuffer.h"
#include "Resource/Public/Resource.h"

// ============================================================================
// RenderCommand
// ----------------------------------------------------------------------------
// Renderer 도메인 command 정의입니다.
//
// 책임:
//   - Renderer가 Backend에게 전달할 draw / present 계열 명령 정의
//   - RenderCommand payload ABI 정의
//
// 비책임:
//   - Resource upload / destroy
//   - File I/O
//   - Codec decode
//   - D3D11 API 호출
// ============================================================================

enum class RenderCommandType : uint32
{
	None = 0,
	ClearRenderTarget = 1,
	Present = 2,
	DrawTexturedQuad = 3
};

using RenderCommandWord = CommandWord;
using RenderCommandHeader = CommandHeader;
using RenderCommandList = CommandList;
using RenderCommandBuffer = CommandBuffer<RenderCommandType>;

struct ClearRenderTargetCommand
{
	float color[4] = {};
};

struct DrawTexturedQuadCommand
{
	ResourceId textureId = InvalidResourceId;
};

static_assert(sizeof(ClearRenderTargetCommand) == 16, "ClearRenderTargetCommand must be 16 bytes.");
static_assert(sizeof(DrawTexturedQuadCommand) == 8, "DrawTexturedQuadCommand must be 8 bytes.");
