#pragma once
// 빌드 시스템이 TARGET_PLATFORM_* 매크로를 주입한다.
// 추후 빌드 시스템(CMake 등) 구축 시 설정 파일에서 읽어 자동 주입 예정.
// 현재는 vcxproj PreprocessorDefinitions에서 수동 주입.

#if defined(TARGET_PLATFORM_WINDOWS)
#define PLATFORM_WINDOWS 1

#elif defined(TARGET_PLATFORM_LINUX)
#define PLATFORM_LINUX 1

#elif defined(TARGET_PLATFORM_ANDROID)
#define PLATFORM_ANDROID 1

#elif defined(TARGET_PLATFORM_MAC)
#define PLATFORM_MAC 1

#else
#error "No target platform defined. Please define TARGET_PLATFORM_* in build settings."
#endif
