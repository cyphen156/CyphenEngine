#include <new>

#include "Dx11Renderer.h"
#include "Modules/Public/ModuleExport.h"
#include "Modules/Renderer/Public/RendererModule.h"

namespace
{
	RendererModuleResult CreateRenderer(
		void* nativeRenderContextHandle,
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

		if (renderer->Initialize(nativeRenderContextHandle, *windowInfo) == false)
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

	RendererModuleResult ExecuteCommandList(
		RendererHandle rendererHandle,
		const RenderCommandList* commandList)
	{
		Dx11Renderer* renderer = static_cast<Dx11Renderer*>(rendererHandle);

		if (renderer == nullptr ||
			commandList == nullptr)
		{
			return RendererModuleResult::Failure;
		}

		if (renderer->ExecuteCommandList(*commandList) == false)
		{
			return RendererModuleResult::Failure;
		}

		return RendererModuleResult::Success;
	}
}

#ifdef _DEBUG
RendererModuleResult ExecuteDebugResourceCommandList(
	RendererHandle rendererHandle,
	const ResourceCommandList* commandList)
{
	Dx11Renderer* renderer = static_cast<Dx11Renderer*>(rendererHandle);

	if (renderer == nullptr ||
		commandList == nullptr)
	{
		return RendererModuleResult::Failure;
	}

	if (renderer->ExecuteResourceCommandList(*commandList) == false)
	{
		return RendererModuleResult::Failure;
	}

	return RendererModuleResult::Success;
}
#endif

// Renderer module entry point입니다.
// Export 속성은 PlatformDefine.h 기반 CYPHEN_MODULE_EXPORT로 분기합니다.
extern "C" CYPHEN_MODULE_EXPORT
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
	outRendererModuleApi->executeCommandList = &ExecuteCommandList;
#ifdef _DEBUG
	outRendererModuleApi->executeDebugResourceCommandList = &ExecuteDebugResourceCommandList;
#endif
	return RendererModuleResult::Success;
}
