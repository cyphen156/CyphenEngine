#include "pch.h"
#include "Engine/Public/CyphenEngine.h"

#include "Core/Public/Path.h"
#include "Core/Public/Time.h"
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
	return engineStatus;
}

bool CyphenEngine::InitEngine()
{
	if (engineStatus != Initializing)
	{
		return false;
	}

	Path::Init();
	Time::Init();
	Renderer::Initialize();

	engineStatus = Ready;

	return true;
}

void CyphenEngine::Run()
{
	if (engineStatus == Ready)
	{
		engineStatus = Running;
	}

	if (engineStatus != Running)
	{
		return;
	}

	Time::Update();

	// TODO:
	// Module Update
	// Runtime Update
	// Editor Update
}

void CyphenEngine::ShutdownEngine()
{
	if (engineStatus == Terminated)
	{
		return;
	}

	Renderer::Shutdown();

	engineStatus = Terminated;
}
