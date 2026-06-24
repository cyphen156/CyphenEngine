#include "pch.h"

#include "Modules/Renderer/Private/RenderCommandBuffer.h"

void RenderCommandBuffer::Reset()
{
	commandBuffer.Reset();
}

bool RenderCommandBuffer::AppendClearRenderTarget(
	float red,
	float green,
	float blue,
	float alpha)
{
	ClearRenderTargetCommand command = {};
	command.color[0] = red;
	command.color[1] = green;
	command.color[2] = blue;
	command.color[3] = alpha;

	return commandBuffer.AppendCommand(
		static_cast<uint32>(RenderCommandType::ClearRenderTarget),
		&command,
		static_cast<uint32>(sizeof(command)));
}

bool RenderCommandBuffer::AppendPresent()
{
	return commandBuffer.AppendCommand(
		static_cast<uint32>(RenderCommandType::Present),
		nullptr,
		0);
}

RenderCommandList RenderCommandBuffer::GetCommandList() const
{
	return commandBuffer.GetCommandList();
}
