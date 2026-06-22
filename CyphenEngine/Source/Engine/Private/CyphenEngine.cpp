#include "pch.h"

#include "Core/Public/Time.h"
#include "Engine/Public/CyphenEngine.h"

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

	engineContext.windowInfo = launchContext.windowInfo;

	if (Time::Init() == false)
	{
		return false;
	}

	if (renderer.Initialize(engineContext.windowInfo) == false)
	{
		return false;
	}

	engineStatus.store(Ready);

	return true;
}

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

	// Renderer 내부에서 Render Thread와 GPU 인스턴스를 먼저 종료합니다.
	renderer.Shutdown();

	engineStatus.store(Terminated);
}

bool CyphenEngine::RequestShutdown()
{
	EngineStatus currentStatus = engineStatus.load();

	while (currentStatus != Terminating &&
		currentStatus != Terminated)
	{
		if (engineStatus.compare_exchange_strong(currentStatus, Terminating))
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
