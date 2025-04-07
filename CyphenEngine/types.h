#pragma once

enum Status
{
	Stopped,
	Paused,

	Running
};

enum class RendererType
{
	DX11,
	DX12,
	VULKAN,
	METAL,
	GDI_PLUS,

	NONE
};