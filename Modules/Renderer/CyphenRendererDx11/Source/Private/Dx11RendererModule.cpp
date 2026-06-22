#include <new>

#include "../Public/Dx11Renderer.h"
#include "Modules/Renderer/Public/RendererModule.h"

namespace
{
	RendererModuleResult CreateRenderer(
		const NativeWindowInfo* windowInfo,
		RendererHandle* outRendererHandle)
	{
		if (windowInfo == nullptr ||
			outRendererHandle == nullptr ||
			*outRendererHandle != nullptr)
		{
			return RendererModuleResult::Failure;
		}

		Dx11Renderer* renderer = new (std::nothrow) Dx11Renderer();

		if (renderer == nullptr)
		{
			return RendererModuleResult::Failure;
		}

		if (renderer->Initialize(*windowInfo) == false)
		{
			delete renderer;
			return RendererModuleResult::Failure;
		}

		*outRendererHandle = renderer;

		return RendererModuleResult::Success;
	}

	void DestroyRenderer(RendererHandle rendererHandle)
	{
		Dx11Renderer* renderer = static_cast<Dx11Renderer*>(rendererHandle);

		if (renderer == nullptr)
		{
			return;
		}

		renderer->Shutdown();
		delete renderer;
	}
}

extern "C" __declspec(dllexport)
RendererModuleResult GetRendererModuleApi(RendererModuleApi* outRendererModuleApi)
{
	if (outRendererModuleApi == nullptr)
	{
		return RendererModuleResult::Failure;
	}

	outRendererModuleApi->apiVersion = RENDERER_MODULE_API_VERSION;
	outRendererModuleApi->rendererType = RendererType::Dx11;
	outRendererModuleApi->createRenderer = &CreateRenderer;
	outRendererModuleApi->destroyRenderer = &DestroyRenderer;

	return RendererModuleResult::Success;
}