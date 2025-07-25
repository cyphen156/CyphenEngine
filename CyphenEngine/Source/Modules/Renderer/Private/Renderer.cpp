#include "pch.h"
#include "Modules/Renderer/Public/Renderer.h"
#include "Common/Public/Logger.h"

namespace Renderer
{
	// Initialize the renderer type based on the defined renderer
	static RendererType rendererType = RendererType::NONE;
	static bool initialized = false;
	static TSTRING rendererName = RendererNames[static_cast<size_t>(rendererType)];

	void Initialize()
	{
		if (initialized)
		{
			LOG_INTERNAL(LogLevel::Error, "Renderer is already initialized.");
			return;
		}

		#if defined(RENDERER_DX11)
			rendererType = RendererType::Dx11;

		#elif defined(RENDERER_DX12)
			rendererType = RendererType::Dx12;

		#elif defined(RENDERER_VULKAN)
			rendererType = RendererType::Vulkan;

		#elif defined(RENDERER_OPENGL)
			rendererType = RendererType::OpenGL;

		#elif defined(RENDERER_METAL)
			rendererType = RendererType::Metal;

		#elif defined(RENDERER_GDI_PLUS)
			rendererType = RendererType::GDI_Plus;
		#else
			LOG_INTERNAL(LogLevel::Error, TTEXT("No Renderer defined. Please define a renderer."));
			rendererType = RendererType::NONE;
		#endif
			rendererName = RendererNames[static_cast<size_t>(rendererType)];
			initialized = true;
			LOG_INTERNAL_T(LogLevel::Info, TTEXT("Renderer initialized: ") + rendererName);
	}

	void Shutdown() 
	{
		initialized = false;
		rendererType = RendererType::NONE;
		rendererName = TTEXT("UNKNOWN");
	}

	RendererType GetRendererType()
	{
		return rendererType;
	}

	const TSTRING& GetRendererName()
	{
		return rendererName;
	}

	bool IsInitialized()
	{
		return initialized;
	}
}