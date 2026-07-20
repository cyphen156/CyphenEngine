#include "pch.h"

#include "Runtime/Public/GameRuntime.h"

bool GameRuntime::Initialize()
{
	if (isInitialized)
	{
		return false;
	}

	world.Reset();
	isInitialized = true;

	return true;
}

void GameRuntime::Tick(double deltaSeconds)
{
	if (isInitialized == false || deltaSeconds < 0.0)
	{
		return;
	}

	GlobalUpdate(deltaSeconds);
	ProcessBeforeWorldTicks(deltaSeconds);

	world.Tick(deltaSeconds);

	ProcessAfterWorldTicks(deltaSeconds);
	GlobalFinalUpdate(deltaSeconds);
}

void GameRuntime::GlobalUpdate(double deltaSeconds)
{
	// Runtime 전체 범위의 OOP 선행 실행 계약을 확정한 이후 구현합니다.
	// 현재는 GlobalPreUpdate 단계의 경계와 delta time 전달만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::ProcessBeforeWorldTicks(double deltaSeconds)
{
	// World 실행 전에 필요한 Runtime-global System 처리 계약을
	// 확정한 이후 구현합니다.
	// 현재는 모든 World Tick 이전의 Process 단계 경계만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::ProcessAfterWorldTicks(double deltaSeconds)
{
	// 모든 World 실행 결과를 사용하는 Runtime-global System 처리 계약을
	// 확정한 이후 구현합니다.
	// 현재는 모든 World Tick 이후의 Process 단계 경계만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::GlobalFinalUpdate(double deltaSeconds)
{
	// Runtime 전체 범위의 OOP 후행 실행 계약을 확정한 이후 구현합니다.
	// 현재는 GlobalFinalUpdate 단계의 경계와 delta time 전달만 보장합니다.
	static_cast<void>(deltaSeconds);
}

void GameRuntime::Shutdown()
{
	if (isInitialized == false)
	{
		return;
	}

	world.Reset();
	isInitialized = false;
}

const World& GameRuntime::GetWorld() const
{
	return world;
}

bool GameRuntime::IsInitialized() const
{
	return isInitialized;
}
