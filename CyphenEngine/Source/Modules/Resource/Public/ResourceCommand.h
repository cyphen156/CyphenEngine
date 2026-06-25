#pragma once

#include "Modules/Public/Command.h"
#include "Modules/Public/CommandBuffer.h"
#include "Resource/Public/Resource.h"
#include "Resource/Public/Texture.h"

// ============================================================================
// ResourceCommand
// ----------------------------------------------------------------------------
// Resource 적재 / 해제 도메인 command 정의입니다.
//
// 책임:
//   - Resource upload / destroy command type 정의
//   - Resource command payload ABI 정의
//
// 비책임:
//   - Render draw command 정의
//   - File I/O
//   - Codec decode
//   - Backend 내부 table layout 결정
// ============================================================================

enum class ResourceCommandType : uint32
{
	None = 0,
	UploadResource = 1,
	DestroyResource = 2
};

using ResourceCommandWord = CommandWord;
using ResourceCommandHeader = CommandHeader;
using ResourceCommandList = CommandList;
using ResourceCommandBuffer = CommandBuffer<ResourceCommandType>;

struct UploadResourceCommand
{
	ResourceId resourceId = InvalidResourceId;
	ResourceKind resourceKind = ResourceKind::Unknown;
	uint64 payloadByteCount = 0;
	// Resource-specific payload follows this struct in command payload.
};

struct DestroyResourceCommand
{
	ResourceId resourceId = InvalidResourceId;
};

struct Texture2DUploadPayload
{
	TextureFormat format = TextureFormat::Unknown;
	uint32 width = 0;
	uint32 height = 0;
};

static_assert(sizeof(UploadResourceCommand) == 24, "UploadResourceCommand must be 24 bytes.");
static_assert(sizeof(DestroyResourceCommand) == 8, "DestroyResourceCommand must be 8 bytes.");
static_assert(sizeof(Texture2DUploadPayload) == 12, "Texture2DUploadPayload must be 12 bytes.");
