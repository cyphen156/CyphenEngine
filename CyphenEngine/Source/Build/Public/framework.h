﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#if defined(PLATFORM_WINDOWS)
	#include "targetver.h"
	#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
	// Windows 헤더 파일
	#include "Platform/windows/Public/LaunchWindows.h"
#elif defined(PLATFORM_LINUX)
	#include "Platform/Linux/Public/LaunchLinux.h"
#else 
	#include "Platform/Independent/public/LaunchIndependent.h"
#endif

