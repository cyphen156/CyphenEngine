#include "pch.h"

#include "Core/Public/CChar.h"
#include "Modules/Renderer/Public/Renderer.h"

#ifdef _DEBUG
#include "Core/Public/Time.h"
#endif

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

	if (moduleBinder.Bind(RendererModuleName) == false)
	{
		return false;
	}

	ModuleSymbol moduleSymbol = moduleBinder.FindSymbol(GET_RENDERER_MODULE_API_NAME);

	if (moduleSymbol == nullptr)
	{
		ReleaseModule();
		return false;
	}

	GetRendererModuleApiFunction getRendererModuleApi =
		reinterpret_cast<GetRendererModuleApiFunction>(moduleSymbol);

	RendererModuleApi resolvedModuleApi = {};

	if (getRendererModuleApi(&resolvedModuleApi) != RendererModuleResult::Success)
	{
		ReleaseModule();
		return false;
	}

	if (resolvedModuleApi.apiVersion != RENDERER_MODULE_API_VERSION)
	{
		ReleaseModule();
		return false;
	}

	if (resolvedModuleApi.rendererType == RendererType::None ||
		resolvedModuleApi.createRenderer == nullptr ||
		resolvedModuleApi.destroyRenderer == nullptr ||
		resolvedModuleApi.executeCommandList == nullptr)
	{
		ReleaseModule();
		return false;
	}

	moduleApi = resolvedModuleApi;

	StartFrameInput();
	SetThreadState(RendererThreadState::Starting);

	if (renderThread.Start(&Renderer::Run, this, windowInfo) == false)
	{
		StopFrameInput();
		SetThreadState(RendererThreadState::Stopped);
		ReleaseModule();
		return false;
	}

	RendererThreadState startupState = RendererThreadState::Starting;

	{
		std::unique_lock<std::mutex> lock(threadMutex);

		while (threadState.load() != RendererThreadState::Running &&
			threadState.load() != RendererThreadState::Failed)
		{
			threadCondition.wait(lock);
		}

		startupState = threadState.load();
	}

	if (startupState == RendererThreadState::Failed)
	{
		renderThread.Join();

		StopFrameInput();
		SetThreadState(RendererThreadState::Stopped);
		ReleaseModule();

		return false;
	}

	return true;
}

void Renderer::Shutdown()
{
	StopFrameInput();

	{
		std::lock_guard<std::mutex> lock(threadMutex);

		if (threadState.load() == RendererThreadState::Running)
		{
			threadState.store(RendererThreadState::Stopping);
		}
	}

	threadCondition.notify_all();

	renderThread.Join();

	SetThreadState(RendererThreadState::Stopped);
	ReleaseModule();
}

bool Renderer::BeginRenderingFrame(const Frame& frame)
{
	if (IsInitialized() == false)
	{
		return false;
	}

	return EnqueueFrame(frame);
}

#ifdef _DEBUG
RendererHandle Renderer::GetDebugRendererHandle() const
{
	return debugRendererHandle;
}

bool Renderer::ExecuteDebugResourceCommandList(const ResourceCommandBuffer& commandBuffer)
{
	const ResourceCommandList commandList = commandBuffer.GetCommandList();

	if (debugRendererHandle == nullptr ||
		commandList.words == nullptr ||
		commandList.wordCount == 0 ||
		commandList.commandCount == 0 ||
		moduleApi.executeDebugResourceCommandList == nullptr)
	{
		return false;
	}

	return moduleApi.executeDebugResourceCommandList(debugRendererHandle, &commandList) ==
		RendererModuleResult::Success;
}
#endif

bool Renderer::IsInitialized() const
{
	return moduleBinder.IsBound() &&
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

#ifdef _DEBUG
	debugRendererHandle = rendererHandle;

	double lastRendererLogTime = Time::ElapsedTime();
	uint64 renderedFrameCount = 0;
	uint64 lastRendererLogFrameCount = 0;
#endif

	SetThreadState(RendererThreadState::Running);
	threadCondition.notify_all();

	Frame currentFrame = {};

	// Render Thread 전용 scratch buffer입니다.
	// 매 프레임 새 vector capacity를 만들지 않고 Reset 후 재사용합니다.
	RenderCommandBuffer commandBuffer;

	while (AcquireFrame(currentFrame))
	{
		if (BuildRenderCommandList(currentFrame, commandBuffer) == false)
		{
			StopFrameInput();
			SetThreadState(RendererThreadState::Failed);
			threadCondition.notify_all();
			break;
		}

		if (ExecuteRenderCommandList(rendererHandle, commandBuffer) == false)
		{
			StopFrameInput();
			SetThreadState(RendererThreadState::Failed);
			threadCondition.notify_all();
			break;
		}

#ifdef _DEBUG
		// Debug Log Per Sec(FPS)
		++renderedFrameCount;

		const double currentRendererLogTime = Time::ElapsedTime();
		const double rendererLogDeltaTime = currentRendererLogTime - lastRendererLogTime;

		if (rendererLogDeltaTime >= 1.0)
		{
			const uint64 renderedFrameDelta =
				renderedFrameCount - lastRendererLogFrameCount;

			const double rendererFrameRate =
				static_cast<double>(renderedFrameDelta) / rendererLogDeltaTime;

			char message[160] = {};
			std::snprintf(
				message,
				sizeof(message),
				"[Renderer] RenderedFrames=%llu FPS=%.2f ElapsedTime=%.6f\n",
				static_cast<unsigned long long>(renderedFrameDelta),
				rendererFrameRate,
				currentRendererLogTime);

			OutputDebugStringA(message);

			lastRendererLogTime = currentRendererLogTime;
			lastRendererLogFrameCount = renderedFrameCount;
		}
#endif
	}

#ifdef _DEBUG
	debugRendererHandle = nullptr;
#endif

	moduleApi.destroyRenderer(rendererHandle);

	SetThreadState(RendererThreadState::Stopped);
	threadCondition.notify_all();
}

bool Renderer::EnqueueFrame(const Frame& frame)
{
	{
		std::unique_lock<std::mutex> lock(frameMutex);

		while (isAcceptingFrames && hasPendingFrame)
		{
			frameCondition.wait(lock);
		}

		if (isAcceptingFrames == false)
		{
			return false;
		}

		pendingFrame = frame;
		hasPendingFrame = true;
	}

	frameCondition.notify_all();

	return true;
}

bool Renderer::AcquireFrame(Frame& outFrame)
{
	{
		std::unique_lock<std::mutex> lock(frameMutex);

		while (isAcceptingFrames && hasPendingFrame == false)
		{
			frameCondition.wait(lock);
		}

		if (hasPendingFrame == false)
		{
			return false;
		}

		outFrame = pendingFrame;
		pendingFrame = {};
		hasPendingFrame = false;
	}

	frameCondition.notify_all();

	return true;
}

bool Renderer::BuildRenderCommandList(const Frame& currentFrame, RenderCommandBuffer& outCommandBuffer)
{
	outCommandBuffer.Reset();

	ClearRenderTargetCommand clearCommand = {};
	clearCommand.color[0] = 0.05f;
	clearCommand.color[1] = 0.08f;
	clearCommand.color[2] = 0.12f;
	clearCommand.color[3] = 1.0f;

	if (outCommandBuffer.AppendCommand(
		RenderCommandType::ClearRenderTarget,
		&clearCommand,
		static_cast<uint32>(sizeof(clearCommand))) == false)
	{
		return false;
	}

	for (const TexturedQuadDrawItem& drawItem : currentFrame.texturedQuadDrawItems)
	{
		DrawTexturedQuadCommand drawCommand = {};
		drawCommand.textureId = drawItem.textureId;

		if (outCommandBuffer.AppendCommand(
			RenderCommandType::DrawTexturedQuad,
			&drawCommand,
			static_cast<uint32>(sizeof(drawCommand))) == false)
		{
			return false;
		}
	}

	return outCommandBuffer.AppendCommand(RenderCommandType::Present, nullptr, 0);
}

bool Renderer::ExecuteRenderCommandList(
	RendererHandle rendererHandle,
	const RenderCommandBuffer& commandBuffer)
{
	const RenderCommandList commandList = commandBuffer.GetCommandList();

	if (commandList.words == nullptr ||
		commandList.wordCount == 0 ||
		commandList.commandCount == 0)
	{
		return false;
	}

	return moduleApi.executeCommandList(rendererHandle, &commandList) ==
		RendererModuleResult::Success;
}

void Renderer::StartFrameInput()
{
	std::lock_guard<std::mutex> lock(frameMutex);

	pendingFrame = {};
	hasPendingFrame = false;
	isAcceptingFrames = true;
}

void Renderer::StopFrameInput()
{
	{
		std::lock_guard<std::mutex> lock(frameMutex);

		pendingFrame = {};
		hasPendingFrame = false;
		isAcceptingFrames = false;
	}

	frameCondition.notify_all();
}

void Renderer::ReleaseModule()
{
	// ModuleBinder.Release() 이후에는 구현 DLL이 Unload될 수 있습니다.
	// DLL에서 얻은 함수 포인터가 Renderer에 남지 않도록 먼저 API 테이블을 비웁니다.
	moduleApi = {};

	const bool isReleased = moduleBinder.Release();

#ifdef _DEBUG
	_ASSERT(isReleased);
#endif

	(void)isReleased;
}

void Renderer::SetThreadState(RendererThreadState state)
{
	std::lock_guard<std::mutex> lock(threadMutex);
	threadState.store(state);
}
