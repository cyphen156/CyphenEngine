#pragma once

#include "Modules/Public/ModuleCommand.h"

// ============================================================================
// RenderCommand IR
// ----------------------------------------------------------------------------
// Renderer와 Backend 구현체 사이의 Command IR입니다.
//
// ModuleCommand:
//   - ABI-safe command stream 운반 포맷입니다.
//
// RenderCommand:
//   - ModuleCommand 포맷 위에 올라가는 Renderer 도메인 명령 의미입니다.
//   - 그래픽스 API 호출 의미를 담습니다.
//     예: ClearRenderTarget / Present / Draw / Dispatch
// ============================================================================
enum class RenderCommandType : uint32
{
	None = 0,
	ClearRenderTarget = 1,
	Present = 2
};

using RenderCommandWord = ModuleCommandWord;
using RenderCommandHeader = ModuleCommandHeader;
using RenderCommandList = ModuleCommandList;

struct ClearRenderTargetCommand
{
	float color[4] = {};
};

static_assert(sizeof(ClearRenderTargetCommand) == 16, "ClearRenderTargetCommand must be 16 bytes.");
