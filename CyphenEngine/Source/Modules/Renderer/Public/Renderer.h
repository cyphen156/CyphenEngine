#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "Core/Public/ModuleBinding.h"
#include "Core/Public/Thread.h"
#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RendererModule.h"

// ============================================================================
// Renderer
// ----------------------------------------------------------------------------
// CyphenEngine이 소유하는 논리 Renderer 시스템입니다.
//
// 책임:
//   - 선택 Renderer 구현 Binding
//   - RendererModuleApi 조회 및 검증
//   - Render Thread 시작·대기·종료
//   - Render Thread에서 구현 인스턴스 생성·파괴
//
// 비책임:
//   - Descriptor와 Preference 생성
//   - Native Binary 직접 Load/Unload
//   - Native Window 생성·소유
//   - DX11/Vulkan API 직접 호출
//
// #2_4 범위:
//   - Renderer 객체화
//   - Render Thread
//   - 구현 GPU 인스턴스 수명
//
// 이월:
//   - Command Queue
//   - RenderCommandList
//   - ExecuteCommandList
//   - Capability
//   - PipelineDescriptor
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

	bool IsInitialized() const;
	RendererType GetRendererType() const;

private:
	void Run(NativeWindowInfo windowInfo);
	void RollbackInitialization();
	void SetThreadState(RendererThreadState state);

private:
	ModuleBinding moduleBinding;
	RendererModuleApi moduleApi = {};

	Thread renderThread;

	std::atomic<RendererThreadState> threadState;
	std::mutex threadMutex;
	std::condition_variable threadCondition;
};
