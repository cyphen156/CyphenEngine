#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "Core/Public/Thread.h"
#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Public/ModuleBinder.h"
#include "Modules/Renderer/Public/Frame.h"
#include "Modules/Renderer/Public/RendererModule.h"
#include "Modules/Renderer/Public/RenderCommand.h"

#ifdef _DEBUG
#include "Modules/Resource/Public/ResourceCommand.h"
#endif

// ============================================================================
// Renderer
// ----------------------------------------------------------------------------
// Engine 측 Renderer 시스템입니다.
//
// 책임:
//   - 선택된 Renderer 구현 Binding
//   - RendererModuleApi 검증
//   - Render Thread 소유
//   - Frame을 받아 Render Thread로 전달
//   - Render Thread에서 RenderCommand IR 생성
//
// 비책임:
//   - World 상태 소유
//   - Native Binary 직접 Load / Unload
//   - DX11 / Vulkan API 직접 호출
// ============================================================================

enum class RendererThreadState : uint32
{
	Stopped,
	Starting,
	Running,
	Failed,
	Stopping
};

class Renderer final
{
public:
	Renderer();
	~Renderer();

	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;

	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	bool Initialize(const NativeWindowInfo& windowInfo);
	void Shutdown();

	bool BeginRenderingFrame(const Frame& frame);

#ifdef _DEBUG
public:
	RendererHandle GetDebugRendererHandle() const;
	bool ExecuteDebugResourceCommandList(const ResourceCommandBuffer& commandBuffer);
private:
	RendererHandle debugRendererHandle = nullptr;
#endif

	bool IsInitialized() const;
	RendererType GetRendererType() const;

private:
	void Run(NativeWindowInfo windowInfo);

	bool EnqueueFrame(const Frame& frame);
	bool AcquireFrame(Frame& outFrame);

	bool BuildRenderCommandList(const Frame& currentFrame, RenderCommandBuffer& outCommandBuffer);
	bool ExecuteRenderCommandList(RendererHandle rendererHandle, const RenderCommandBuffer& commandBuffer);

	void StartFrameInput();
	void StopFrameInput();

	void ReleaseModule();
	void SetThreadState(RendererThreadState state);

private:
	ModuleBinder moduleBinder;
	RendererModuleApi moduleApi = {};

	Thread renderThread;

	std::atomic<RendererThreadState> threadState;
	std::mutex threadMutex;
	std::condition_variable threadCondition;

	std::mutex frameMutex;
	std::condition_variable frameCondition;
	Frame pendingFrame = {};
	bool hasPendingFrame = false;
	bool isAcceptingFrames = false;
};
