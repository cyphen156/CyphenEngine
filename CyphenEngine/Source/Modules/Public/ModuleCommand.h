#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// ModuleCommand
// ----------------------------------------------------------------------------
// Module 시스템에서 사용할 ABI-safe command stream 공통 포맷입니다.
//
// 이 파일은 command stream의 "운반 형식"만 정의합니다.
// 각 Module 도메인의 command 의미는 여기서 정의하지 않습니다.
//
// 예:
//   - Renderer는 RenderCommandType / ClearRenderTargetCommand를 정의합니다.
//   - Audio는 AudioCommandType / SubmitSamplesCommand를 정의할 수 있습니다.
//
// 규칙:
//   - 64-bit word stream입니다.
//   - Header는 정확히 1 word입니다.
//   - Payload는 payloadWordCount만큼 뒤따릅니다.
//   - ABI 경계에는 STL 컨테이너, 가상 객체, 소유 포인터를 넘기지 않습니다.
// ============================================================================
using ModuleCommandWord = uint64;

struct ModuleCommandHeader
{
	uint32 commandType = 0;
	uint32 payloadWordCount = 0;
};

struct ModuleCommandList
{
	const ModuleCommandWord* words = nullptr;
	uint32 wordCount = 0;
	uint32 commandCount = 0;
};

static_assert(sizeof(ModuleCommandWord) == 8, "ModuleCommandWord must be 8 bytes.");
static_assert(sizeof(ModuleCommandHeader) == 8, "ModuleCommandHeader must be 8 bytes.");
static_assert(sizeof(ModuleCommandList) == 16, "ModuleCommandList must be 16 bytes.");
