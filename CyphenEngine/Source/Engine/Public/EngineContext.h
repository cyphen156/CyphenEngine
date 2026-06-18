#pragma once

#include "Core/Public/CPrimitiveTypes.h"

/// <summary>
/// Launch가 수집하여 엔진 초기화에 전달하는 실행 컨텍스트입니다.
/// </summary>
struct LaunchContext
{
	void* nativeWindowHandle = nullptr;
	uint32 windowWidth = 0;
	uint32 windowHeight = 0;
};

/// <summary>
/// 엔진이 초기화 후 보관하는 실행 컨텍스트입니다.
/// </summary>
struct EngineContext
{
	void* nativeWindowHandle = nullptr;
	uint32 windowWidth = 0;
	uint32 windowHeight = 0;
};
