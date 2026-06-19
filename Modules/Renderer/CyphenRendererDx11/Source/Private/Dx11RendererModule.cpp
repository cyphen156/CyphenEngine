#include "Modules/Renderer/Public/RendererModule.h"

extern "C" __declspec(dllexport)
RendererModuleResult GetRendererModuleApi(RendererModuleApi* outRendererModuleApi)
{
	if (outRendererModuleApi == nullptr)
	{
		return RendererModuleResult::Failure;
	}

	outRendererModuleApi->apiVersion = RENDERER_MODULE_API_VERSION;
	outRendererModuleApi->rendererType = RendererType::Dx11;

	return RendererModuleResult::Success;
}