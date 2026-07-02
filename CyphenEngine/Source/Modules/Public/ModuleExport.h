#pragma once

#include "Build/Public/PlatformDefine.h"

#if PLATFORM_WINDOWS
#define CYPHEN_MODULE_EXPORT __declspec(dllexport)
#elif PLATFORM_LINUX
#define CYPHEN_MODULE_EXPORT __attribute__((visibility("default")))
#else
#define CYPHEN_MODULE_EXPORT
#endif
