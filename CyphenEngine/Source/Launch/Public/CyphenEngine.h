#pragma once

// �÷��� Ȯ��
#if defined(PLATFORM_WINDOWS)
	#include "Platform/Windows/Public/LaunchWindows.h"
#elif defined(PLATFORM_LINUX)
	#include "Platform/Linux/Public/LaunchLinux.h"
#endif
