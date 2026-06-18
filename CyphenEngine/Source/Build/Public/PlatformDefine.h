#pragma once

// ============================================================================
// PlatformDefine
// ----------------------------------------------------------------------------
// 빌드 시스템이 주입한 TARGET_PLATFORM_* 매크로를 기준으로
// 엔진 내부에서 사용할 PLATFORM_* 매크로를 확정합니다.
//
// 현재는 vcxproj PreprocessorDefinitions에서 TARGET_PLATFORM_WINDOWS를
// 수동 주입합니다.
// ============================================================================

#if (defined(TARGET_PLATFORM_WINDOWS) + defined(TARGET_PLATFORM_LINUX) + defined(TARGET_PLATFORM_ANDROID) + defined(TARGET_PLATFORM_MAC)) != 1
#error "Exactly one TARGET_PLATFORM_* must be defined in build settings."
#endif

#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_MAC 0

#if defined(TARGET_PLATFORM_WINDOWS)
#undef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS 1

#elif defined(TARGET_PLATFORM_LINUX)
#undef PLATFORM_LINUX
#define PLATFORM_LINUX 1

#elif defined(TARGET_PLATFORM_ANDROID)
#undef PLATFORM_ANDROID
#define PLATFORM_ANDROID 1

#elif defined(TARGET_PLATFORM_MAC)
#undef PLATFORM_MAC
#define PLATFORM_MAC 1

#endif
