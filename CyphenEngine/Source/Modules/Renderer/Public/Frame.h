#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// Frame
// ----------------------------------------------------------------------------
// Renderer에 제출되는 공개 프레임 입력 POD입니다.
//
// Engine / Runtime은 현재 World 상태를 렌더링 가능한 형태로 고정한 뒤,
// 그 결과를 Frame으로 만들어 Renderer에 제출합니다.
//
// Renderer는 Frame을 복사해 Render Thread에서 소비하고,
// 이 입력을 기반으로 RenderCommand IR을 생성합니다.
//
// 주의:
//   - Core 전역 타입이 아닙니다.
//   - Renderer 내부 전용 타입도 아닙니다.
//   - Backend DLL로 직접 전달되는 ABI 타입도 아닙니다.
//   - Backend로 내려가는 것은 Frame이 아니라 RenderCommandList입니다.
// ============================================================================
struct Frame
{
	uint64 frameNumber = 0;
};
