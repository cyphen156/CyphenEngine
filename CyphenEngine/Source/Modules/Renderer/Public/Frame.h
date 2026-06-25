#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Resource/Public/Resource.h"

// ============================================================================
// TexturedQuadDrawItem
// ----------------------------------------------------------------------------
// Frame이 참조하는 2D textured quad draw item입니다.
//
// Frame은 이미 만들어진 월드 / 렌더 입력의 스냅샷이므로 texture bytes나
// CPU-side Texture2D payload를 들지 않습니다.
//
// Renderer는 textureId를 DrawTexturedQuad command로 변환하고,
// Backend는 textureId로 GPU resource table을 조회합니다.
// ============================================================================

struct TexturedQuadDrawItem
{
	ResourceId textureId = InvalidResourceId;
};

// ============================================================================
// Frame
// ----------------------------------------------------------------------------
// 특정 시점에 스냅샷된 렌더 입력입니다.
//
// Frame은 hot path에 가까운 데이터이므로 ResourceId 같은 작은 참조만
// 보유합니다. Resource upload / destroy 같은 리소스 수명 이벤트는 Frame
// 자체에서 나오지 않고 별도 pending resource 경로에서 command로 변환됩니다.
// ============================================================================

struct Frame
{
	uint64 frameNumber = 0;
	std::vector<TexturedQuadDrawItem> texturedQuadDrawItems;
};
