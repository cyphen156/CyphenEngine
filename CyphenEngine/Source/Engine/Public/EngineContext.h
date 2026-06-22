#pragma once

#include <vector>

#include "Core/Public/ModuleDescriptor.h"
#include "HAL/Public/NativeWindowInfo.h"

// ============================================================================
// LaunchContext
// ----------------------------------------------------------------------------
// Platform Launch가 Engine 초기화 전에 수집한 실행 정보입니다.
//
// Native Window 정보와 외부에서 결정된 Module 구성을 Engine에 전달합니다.
// ============================================================================

struct LaunchContext
{
	NativeWindowInfo windowInfo;
	std::vector<ModuleDescriptor> moduleDescriptors;
};

// ============================================================================
// EngineContext
// ----------------------------------------------------------------------------
// CyphenEngine이 실행 중 보관하는 환경 정보입니다.
//
// LaunchContext 전체를 보관하지 않고 Engine 실행에 필요한 정보만
// 책임별 공통 타입으로 보관합니다.
// ============================================================================

struct EngineContext
{
	NativeWindowInfo windowInfo;
};
