#pragma once
#include "Core/Public/CChar.h"

enum EngineStatus
{
	Initializing,
	Ready,
	Running,
	Paused,
	Terminating,
	Terminated,

	None
};

constexpr const CChar* RendererNames[] =
{
	CTEXT("Dx11"),
	CTEXT("Dx12"),
	CTEXT("Vulkan"),
	CTEXT("OpenGL"),
	CTEXT("Metal"),
	CTEXT("GDI_Plus"),

	CTEXT("NONE")
};


enum class LogLevel
{
	Info,
	Warning,
	Error,
	Fatal,

	COUNT
};

constexpr const CChar* LogLevelNames[] =
{
	CTEXT("Info"),
	CTEXT("Warning"),
	CTEXT("Error"),
	CTEXT("Fatal")
};
