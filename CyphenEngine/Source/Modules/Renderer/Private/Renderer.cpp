#include "pch.h"

#include <vector>

#include "Core/Public/ModuleManager.h"
#include "Modules/Renderer/Public/Renderer.h"
#include "Modules/Renderer/Public/RendererModule.h"

static CString gRendererModuleName;
static RendererModuleApi gRendererModuleApi = {};
static bool gIsRendererInitialized = false;

bool Renderer::Initialize()
{
	if (gIsRendererInitialized == true)
	{
		return false;
	}

	std::vector<CString> moduleNames;
	ModuleManager::GetLoadedModuleNames(moduleNames);

	CString selectedModuleName;
	RendererModuleApi selectedModuleApi = {};

	for (const CString& moduleName : moduleNames)
	{
		ModuleSymbol moduleSymbol =
			ModuleManager::FindSymbol(moduleName, GET_RENDERER_MODULE_API_NAME);

		if (moduleSymbol == nullptr)
		{
			continue;
		}

		GetRendererModuleApiFunction getRendererModuleApi =
			reinterpret_cast<GetRendererModuleApiFunction>(moduleSymbol);

		RendererModuleApi rendererModuleApi = {};

		if (getRendererModuleApi(&rendererModuleApi) != RendererModuleResult::Success)
		{
			continue;
		}

		if (rendererModuleApi.apiVersion != RENDERER_MODULE_API_VERSION)
		{
			continue;
		}

		if (rendererModuleApi.rendererType == RendererType::None)
		{
			continue;
		}

		if (selectedModuleName.empty() == false)
		{
			return false;
		}

		selectedModuleName = moduleName;
		selectedModuleApi = rendererModuleApi;
	}

	if (selectedModuleName.empty())
	{
		return false;
	}

	gRendererModuleName = selectedModuleName;
	gRendererModuleApi = selectedModuleApi;
	gIsRendererInitialized = true;

	return true;
}

void Renderer::Shutdown()
{
	if (gIsRendererInitialized == false)
	{
		return;
	}

	// 이후 RendererModuleApi에 실행 계약이 추가되면:
	// Renderer Thread 종료 신호
	// Renderer Thread Join
	// RendererModuleApi::Shutdown 호출

	gRendererModuleApi = {};
	gRendererModuleName.clear();
	gIsRendererInitialized = false;
}

bool Renderer::IsInitialized()
{
	return gIsRendererInitialized;
}

RendererType Renderer::GetRendererType()
{
	return gRendererModuleApi.rendererType;
}
