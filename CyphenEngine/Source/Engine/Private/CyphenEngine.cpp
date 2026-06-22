#include "pch.h"

#include "Core/Public/Time.h"
#include "Engine/Public/CyphenEngine.h"
#include "Modules/Renderer/Public/Renderer.h"

CyphenEngine::CyphenEngine()
	: engineStatus(Initializing)
{
}

CyphenEngine::~CyphenEngine()
{
}

EngineStatus CyphenEngine::GetEngineStatus() const
{
	return engineStatus.load();
}

bool CyphenEngine::InitEngine(const LaunchContext& launchContext)
{
	if (engineStatus.load() != Initializing)
	{
		return false;
	}

	// Launch가 수집한 실행 환경 중 Engine이 소유할 값을 주입합니다.
	engineContext.nativeWindowHandle = launchContext.nativeWindowHandle;
	engineContext.windowWidth = launchContext.windowWidth;
	engineContext.windowHeight = launchContext.windowHeight;

	if (Time::Init() == false)
	{
		return false;
	}

	// Core 시스템의 초기화 순서는 Engine이 명시적으로 소유합니다.
	if (Renderer::Initialize() == false)
	{
		return false;
	}

	engineStatus.store(Ready);

	return true;
}

// Engine의 논리 실행 루프입니다.
// Renderer의 독립 실행 루프는 #2_4에서 별도 Render Thread로 추가합니다.
void CyphenEngine::Run()
{
	if (ChangeEngineStatus(Ready, Running) == false)
	{
		if (engineStatus.load() == Terminating)
		{
			ShutdownEngine();
		}

		return;
	}

	while (engineStatus.load() == Running)
	{
		Time::Tick();

		// TODO:
		// BUILD_TARGET 기준 Runtime Tick
	}

	ShutdownEngine();
}

void CyphenEngine::ShutdownEngine()
{
	if (engineStatus.load() == Terminated)
	{
		return;
	}

	// 초기화의 역순으로 Core 시스템을 종료합니다.
	Renderer::Shutdown();

	engineStatus.store(Terminated);
}

bool CyphenEngine::RequestShutdown()
{
	EngineStatus currentStatus = engineStatus.load();

	while (currentStatus != Terminating &&
		currentStatus != Terminated)
	{
		if (engineStatus.compare_exchange_strong(
			currentStatus,
			Terminating))
		{
			return true;
		}
	}

	return false;
}

bool CyphenEngine::ChangeEngineStatus(EngineStatus expected, EngineStatus desired)
{
	return engineStatus.compare_exchange_strong(expected, desired);
}
