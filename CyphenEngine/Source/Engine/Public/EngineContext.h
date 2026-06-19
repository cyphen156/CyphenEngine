#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/ModuleDescriptor.h"

/// <summary>
/// 엔진에게 전달하기 전의 시작 정보입니다.
/// </summary>
struct LaunchContext
{
	void* nativeWindowHandle = nullptr;
	uint32 windowWidth = 0;
	uint32 windowHeight = 0;

	std::vector<ModuleDescriptor> moduleDescriptors;
};

/// <summary>
/// 런타임이 소유하는 실행 환경입니다.
/// </summary>
struct EngineContext
{
	void* nativeWindowHandle = nullptr;
	uint32 windowWidth = 0;
	uint32 windowHeight = 0;
};
