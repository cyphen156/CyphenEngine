#include "pch.h"

#include "Engine/Public/CyphenEngine.h"
#include "Core/Public/Time.h"

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

bool CyphenEngine::InitEngine()
{
	if (engineStatus.load() != Initializing)
	{
		return false;
	}

	if (Time::Init() == false)
	{
		return false;
	}

	engineStatus.store(Ready);

	return true;
}

// 엔진의 메인 루프입니다. Run()이 호출되면 엔진은 Running 상태가 될 때까지 대기한 후, Running 상태가 유지되는 동안 계속해서 루프를 실행합니다.
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

	engineStatus.store(Terminated);
}

bool CyphenEngine::RequestShutdown()
{
	EngineStatus currentStatus = engineStatus.load();

	while (currentStatus != Terminating && currentStatus != Terminated)
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
