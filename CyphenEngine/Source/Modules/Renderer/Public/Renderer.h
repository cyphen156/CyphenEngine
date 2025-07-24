#pragma once

// Define the Renderer
#if defined(PLATFORM_WINDOWS)
	#define RENDERER_DX11 2
	//#define RENDERER_DX12 2
	//#define RENDERER_VULKAN 2
	//#define RENDERER_OPENGL 2
	//#define RENDERER_GDI_PLUS 2
	#if defined(RENDERER_DX11)
		#include "Modules/Renderer/Public/Dx11Renderer.h"

	#elif defined(RENDERER_DX12)
		#include "Modules/Renderer/Public/Dx12Renderer.h"

	#elif defined(RENDERER_VULKAN)
		#include "Modules/Renderer/Public/VulkanRenderer.h"

	#elif defined(RENDERER_OPENGL)
		#include "Modules/Renderer/Public/OpenGLRenderer.h"

	#elif defined(RENDERER_GDI_PLUS)
		#include "Modules/Renderer/Public/GdiPlusRenderer.h"

	#else
		#error "No Renderer defined. Please define a renderer."
	#endif

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID) || defined(PLATFORM_INDEPENDENT)
	#define RENDERER_VULKAN 2
	//#define RENDERER_OPENGL 2
	#if defined(RENDERER_VULKAN)
		#include "Modules/Renderer/Public/VulkanRenderer.h"

	#elif defined(RENDERER_OPENGL)
		#include "Modules/Renderer/Public/OpenGLRenderer.h"

	#else
		#error "No Renderer defined. Please define a renderer."
	#endif

#elif defined(PLATFORM_MAC)
	#define RENDERER_METAL 2
	#include "Modules/Renderer/Public/MetalRenderer.h"

#else
	#define RENDERER_OPENGL 2
	#include "Modules/Renderer/Public/OpenGLRenderer.h"
#endif

namespace Renderer
{
	void Initialize();
	void Shutdown();

	RendererType GetRendererType();
	const TSTRING& GetRendererName();
	bool IsInitialized();
};