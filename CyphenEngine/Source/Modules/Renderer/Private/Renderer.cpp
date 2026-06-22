#include "pch.h"

#include "Core/Public/CChar.h"
#include "Core/Public/ModuleBinding.h"
#include "Modules/Renderer/Public/Renderer.h"
#include "Modules/Renderer/Public/RendererModule.h"

namespace
{
	constexpr CChar RendererModuleName[] = CTEXT("Renderer");

	// ========================================================================
	// RendererModule
	// ------------------------------------------------------------------------
	// Renderer Facade의 실제 상태를 소유하는 단일 시스템 객체입니다.
	//
	// ModuleBinding과 RendererModuleApi는 이 객체만 소유합니다.
	// 별도 초기화 bool을 두지 않고 Binding과 API 상태에서 연결 상태를
	// 판정합니다.
	// ========================================================================

	class RendererModule final
	{
	public:
		bool Initialize()
		{
			if (IsInitialized())
			{
				return false;
			}

			if (moduleBinding.Bind(RendererModuleName) == false)
			{
				return false;
			}

			ModuleSymbol moduleSymbol =
				moduleBinding.FindSymbol(GET_RENDERER_MODULE_API_NAME);

			if (moduleSymbol == nullptr)
			{
				RollbackBinding();
				return false;
			}

			GetRendererModuleApiFunction getRendererModuleApi =
				reinterpret_cast<GetRendererModuleApiFunction>(moduleSymbol);

			RendererModuleApi resolvedModuleApi = {};

			if (getRendererModuleApi(&resolvedModuleApi) !=
				RendererModuleResult::Success)
			{
				RollbackBinding();
				return false;
			}

			if (resolvedModuleApi.apiVersion != RENDERER_MODULE_API_VERSION)
			{
				RollbackBinding();
				return false;
			}

			if (resolvedModuleApi.rendererType == RendererType::None)
			{
				RollbackBinding();
				return false;
			}

			// 모든 검증이 끝난 뒤에만 활성 API를 확정합니다.
			moduleApi = resolvedModuleApi;

			return true;
		}

		void Shutdown()
		{
			// DLL symbol 참조를 먼저 제거한 뒤 Binary 참조를 해제합니다.
			moduleApi = {};

			const bool isReleased = moduleBinding.Release();

#ifdef _DEBUG
			_ASSERT(isReleased);
#endif

			(void)isReleased;
		}

		bool IsInitialized() const
		{
			return moduleBinding.IsBound() &&
				moduleApi.rendererType != RendererType::None;
		}

		RendererType GetRendererType() const
		{
			return moduleApi.rendererType;
		}

	private:
		void RollbackBinding()
		{
			moduleApi = {};

			const bool isReleased = moduleBinding.Release();

#ifdef _DEBUG
			_ASSERT(isReleased);
#endif

			(void)isReleased;
		}

	private:
		ModuleBinding moduleBinding;
		RendererModuleApi moduleApi = {};
	};

	RendererModule gRendererModule;
}

bool Renderer::Initialize()
{
	return gRendererModule.Initialize();
}

void Renderer::Shutdown()
{
	gRendererModule.Shutdown();
}

bool Renderer::IsInitialized()
{
	return gRendererModule.IsInitialized();
}

RendererType Renderer::GetRendererType()
{
	return gRendererModule.GetRendererType();
}
