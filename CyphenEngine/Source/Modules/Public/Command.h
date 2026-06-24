#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Command
// ----------------------------------------------------------------------------
// Module / Renderer / Resource 계열에서 공유하는 ABI-safe command stream
// 공통 운반 포맷입니다.
//
// 이 파일은 command stream의 저장 형식만 정의합니다.
// commandType의 의미는 각 도메인 command 파일에서 정의합니다.
//
// 규칙:
//   - 64-bit word stream입니다.
//   - Header는 정확히 1 word입니다.
//   - Payload는 payloadWordCount만큼 뒤따릅니다.
//   - ABI 경계에는 STL 컨테이너, 가상 객체, 소유 포인터를 넘기지 않습니다.
// ============================================================================

using CommandWord = uint64;

struct CommandHeader
{
	uint32 commandType = 0;
	uint32 payloadWordCount = 0;
};

struct CommandList
{
	const CommandWord* words = nullptr;
	uint32 wordCount = 0;
	uint32 commandCount = 0;
};

static_assert(sizeof(CommandWord) == 8, "CommandWord must be 8 bytes.");
static_assert(sizeof(CommandHeader) == 8, "CommandHeader must be 8 bytes.");
static_assert(sizeof(CommandList) == 16, "CommandList must be 16 bytes.");
