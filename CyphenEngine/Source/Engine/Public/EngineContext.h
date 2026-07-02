#pragma once

#include <vector>

#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Public/ModuleDescriptor.h"

// ============================================================================
// LaunchContext
// ----------------------------------------------------------------------------
// Platform Launch가 획득한 native render context, main window 정보,
// 외부에서 결정된 module 구성을 Engine에 전달합니다.
// ============================================================================

struct LaunchContext
{
	// Platform Launch가 획득한 대표 render context handle입니다.
	// Windows: HINSTANCE, Linux X11: Display*.
	void* nativeRenderContextHandle = nullptr;

	// Platform Launch가 만든 프로세스 대표 main window 정보입니다.
	NativeWindowInfo mainWindowInfo;

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
