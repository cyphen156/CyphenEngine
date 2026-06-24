#pragma once

#include "Modules/Public/ModuleCommandBuffer.h"
#include "Modules/Renderer/Public/RenderCommand.h"

// ============================================================================
// RenderCommandBuffer
// ----------------------------------------------------------------------------
// Renderer 도메인 명령을 ModuleCommandBuffer에 기록하는 얇은 조합기입니다.
//
// 책임:
//   - RenderCommandType을 uint32 commandType으로 변환
//   - Renderer 도메인 payload 작성
//
// 비책임:
//   - word stream 저장 방식
//   - payload padding
//   - command count 관리
// ============================================================================
class RenderCommandBuffer final
{
public:
	RenderCommandBuffer() = default;
	~RenderCommandBuffer() = default;

	RenderCommandBuffer(const RenderCommandBuffer& other) = delete;
	RenderCommandBuffer& operator=(const RenderCommandBuffer& other) = delete;

	void Reset();

	bool AppendClearRenderTarget(float red, float green, float blue, float alpha);
	bool AppendPresent();

	RenderCommandList GetCommandList() const;

private:
	ModuleCommandBuffer commandBuffer;
};
