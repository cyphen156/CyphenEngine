#pragma once

#include "Core/Public/CPrimitiveTypes.h"

// ============================================================================
// NativeWindowInfo
// ----------------------------------------------------------------------------
// Platform Launch가 생성한 Native Window의 전달 정보입니다.
//
// 이 구조체는 Native Window를 생성하거나 소유하지 않습니다.
// Window Handle과 현재 Client 영역 크기만 전달합니다.
//
// 생성:
//   - Platform Launch
//
// 보관:
//   - LaunchContext
//   - EngineContext
//
// 소비:
//   - Renderer
//   - Renderer 구현 DLL
// ============================================================================

struct NativeWindowInfo
{
	void* nativeWindowHandle = nullptr;
	uint32 windowWidth = 0;
	uint32 windowHeight = 0;
};
