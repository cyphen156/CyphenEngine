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
	DX11,
	DX12,
	VULKAN,
	METAL,
	GDI_PLUS,

	NONE
};

constexpr const TCHAR* RendererNames[] = {
	TTEXT("DX11"),
	TTEXT("DX12"),	
	TTEXT("VULKAN"),
	TTEXT("METAL"),
	TTEXT("GDI_PLUS"),

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