#include "pch.h"

#include "Core/Public/CChar.h"
#include "Modules/Renderer/Public/Renderer.h"

namespace
{
	constexpr CChar RendererModuleName[] = CTEXT("Renderer");
}

Renderer::Renderer()
	: threadState(RendererThreadState::Stopped)
{
}

Renderer::~Renderer()
{
#ifdef _DEBUG
	_ASSERT(renderThread.IsJoinable() == false);
	_ASSERT(threadState.load() == RendererThreadState::Stopped);
#endif
}

bool Renderer::Initialize(const NativeWindowInfo& windowInfo)
{
	if (IsInitialized())
	{
		return false;
	}

	if (windowInfo.nativeWindowHandle == nullptr ||
		windowInfo.windowWidth == 0 ||
		windowInfo.windowHeight == 0)
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
		RollbackInitialization();
		return false;
	}

	GetRendererModuleApiFunction getRendererModuleApi =
		reinterpret_cast<GetRendererModuleApiFunction>(moduleSymbol);

	RendererModuleApi resolvedModuleApi = {};

	if (getRendererModuleApi(&resolvedModuleApi) !=
		RendererModuleResult::Success)
	{
		RollbackInitialization();
		return false;
	}

	if (resolvedModuleApi.apiVersion != RENDERER_MODULE_API_VERSION)
	{
		RollbackInitialization();
		return false;
	}

	if (resolvedModuleApi.rendererType == RendererType::None ||
		resolvedModuleApi.createRenderer == nullptr ||
		resolvedModuleApi.destroyRenderer == nullptr)
	{
		RollbackInitialization();
		return false;
	}

	// 모든 ABI 검증이 끝난 뒤 활성 API를 확정합니다.
	moduleApi = resolvedModuleApi;

	SetThreadState(RendererThreadState::Starting);

	if (renderThread.Start(&Renderer::Run, this, windowInfo) == false)
	{
		SetThreadState(RendererThreadState::Stopped);
		RollbackInitialization();

		return false;
	}

	RendererThreadState startupState;

	{
		std::unique_lock<std::mutex> lock(threadMutex);

		threadCondition.wait(lock, [this]()
			{
				const RendererThreadState state = threadState.load();

				return state == RendererThreadState::Running ||
					state == RendererThreadState::Failed;
			});

		startupState = threadState.load();
	}

	if (startupState == RendererThreadState::Failed)
	{
		renderThread.Join();

		SetThreadState(RendererThreadState::Stopped);
		RollbackInitialization();

		return false;
	}

	return true;
}

void Renderer::Shutdown()
{
	{
		std::lock_guard<std::mutex> lock(threadMutex);

		if (threadState.load() == RendererThreadState::Running)
		{
			threadState.store(RendererThreadState::Stopping);
		}
	}

	threadCondition.notify_all();

	// Thread가 완전히 종료된 뒤 함수 포인터와 Binary 참조를 해제합니다.
	renderThread.Join();

	moduleApi = {};

	const bool isReleased = moduleBinding.Release();

#ifdef _DEBUG
	_ASSERT(isReleased);
#endif

	(void)isReleased;
}

bool Renderer::IsInitialized() const
{
	return moduleBinding.IsBound() &&
		moduleApi.rendererType != RendererType::None &&
		threadState.load() == RendererThreadState::Running;
}

RendererType Renderer::GetRendererType() const
{
	return moduleApi.rendererType;
}

void Renderer::Run(NativeWindowInfo windowInfo)
{
	RendererHandle rendererHandle = nullptr;

	RendererModuleResult createResult =
		moduleApi.createRenderer(&windowInfo, &rendererHandle);

	if (createResult != RendererModuleResult::Success ||
		rendererHandle == nullptr)
	{
		SetThreadState(RendererThreadState::Failed);
		threadCondition.notify_all();

		return;
	}

	SetThreadState(RendererThreadState::Running);
	threadCondition.notify_all();

	{
		std::unique_lock<std::mutex> lock(threadMutex);

		threadCondition.wait(lock, [this]()
			{
				return threadState.load() ==
					RendererThreadState::Stopping;
			});
	}

	moduleApi.destroyRenderer(rendererHandle);

	SetThreadState(RendererThreadState::Stopped);
	threadCondition.notify_all();
}

void Renderer::RollbackInitialization()
{
	moduleApi = {};

	const bool isReleased = moduleBinding.Release();

#ifdef _DEBUG
	_ASSERT(isReleased);
#endif

	(void)isReleased;
}

void Renderer::SetThreadState(RendererThreadState state)
{
	// condition_variable의 predicate와 같은 mutex로 상태를 변경합니다.
	std::lock_guard<std::mutex> lock(threadMutex);
	threadState.store(state);
}
