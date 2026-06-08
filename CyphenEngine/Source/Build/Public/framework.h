#pragma once

// ============================================================================
// Framework
// ----------------------------------------------------------------------------
// 선택된 PLATFORM_* 기준으로 플랫폼별 컴파일 환경을 준비합니다.
//
// - 플랫폼 시스템 헤더 포함
// - 플랫폼별 OS 타입 규약 확정
// ============================================================================
#if defined(PLATFORM_WINDOWS)

	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN

	#include "Platform/Windows/Public/targetver.h"
	#include <Windows.h>

	using LARGEINTEGER = LARGE_INTEGER;

#elif defined(PLATFORM_LINUX)

	#include <cstdint>

	using LARGEINTEGER = int64_t;

#elif defined(PLATFORM_ANDROID)

	#error "Android framework is not implemented."

#elif defined(PLATFORM_MAC)

	#error "Mac framework is not implemented."

#else

	#error "Unsupported platform."

#endif
