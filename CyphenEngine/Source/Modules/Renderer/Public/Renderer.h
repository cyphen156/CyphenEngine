#pragma once

class Renderer
{
public:
	static void InitRenderer(RendererType type);
	static void Shutdown();
	static RendererType GetRendererType();
	static const TSTRING& GetRendererName();
	static const TSTRING& GetRendererVersion();
	static const TSTRING& GetRendererAPIVersion();
	static bool IsInitialized();
};

