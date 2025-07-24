#pragma once

enum EngineStatus
{
	Initializing,
	Ready,
	Running,
	Paused,
	Terminated,
	
	None
};

enum class RendererType
{
	Dx11,
	Dx12,
	Vulkan,
	OpenGL,
	Metal,
	GDI_Plus,

	NONE
};

constexpr const TCHAR* RendererNames[] = {
	TTEXT("Dx11"),
	TTEXT("Dx12"),	
	TTEXT("Vulkan"),
	TTEXT("OpenGL"),
	TTEXT("Metal"),
	TTEXT("GDI_Plus"),

	TTEXT("NONE")
};

enum class LogLevel
{
	Info,
	Warning,
	Error,
	Fatal,

	COUNT
};

constexpr const TCHAR* LogLevelNames[] = {
	TTEXT("Info"),
	TTEXT("Warning"),
	TTEXT("Error"),
	TTEXT("Fatal")
};