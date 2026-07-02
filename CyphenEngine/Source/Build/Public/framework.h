#pragma once

// ============================================================================
// Framework
// ----------------------------------------------------------------------------
// 선택된 PLATFORM_* 기준으로 플랫폼별 컴파일 환경을 준비합니다.
//
// - 플랫폼 시스템 헤더 포함
// - 플랫폼별 OS 타입 규약 확정
// ============================================================================

#include "Build/Public/PlatformDefine.h"

#if PLATFORM_WINDOWS

	#ifdef _DEBUG
		#define _CRTDBG_MAP_ALLOC
		#include <crtdbg.h>
		#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
		#define PRINT_DEBUG_OUTPUT(text) OutputDebugStringA(text)
	#endif

	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN

	#include "Platform/Windows/Public/targetver.h"
	#include <Windows.h>

	using LARGEINTEGER = LARGE_INTEGER;

#elif PLATFORM_LINUX

	#ifdef _DEBUG
		#include <cassert>
		#include <cstdio>
		
		#ifndef  _ASSERT
			#define _ASSERT(expression) assert(expression)
		#endif

		#define PRINT_DEBUG_OUTPUT(text) std::fputs((text), stderr)
	#endif

	#include <cstdint>

	// Linux 디버그 힙 경계 (crtdbg 등가물 없음 → ASan/valgrind, 추후 Diagnostics)

	using LARGEINTEGER = int64_t;

#elif PLATFORM_ANDROID

	#error "Android framework is not implemented."

#elif PLATFORM_MAC

	#error "Mac framework is not implemented."

#else

	#error "Unsupported platform."

#endif
