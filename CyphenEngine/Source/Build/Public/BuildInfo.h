#pragma once

#include "Build/Public/PlatformDefine.h"

namespace Build
{
	enum class BuildTarget : unsigned char
	{
		Editor,
		Game,
		Server
	};

	enum class BuildPlatform : unsigned char
	{
		Windows,
		Linux,
		Android,
		Mac
	};

#if defined(BUILD_TARGET_EDITOR) && (defined(BUILD_TARGET_GAME) || defined(BUILD_TARGET_SERVER))
#error "Only one BUILD_TARGET_* must be defined."

#elif defined(BUILD_TARGET_GAME) && defined(BUILD_TARGET_SERVER)
#error "Only one BUILD_TARGET_* must be defined."

#elif defined(BUILD_TARGET_EDITOR)
	constexpr BuildTarget Target = BuildTarget::Editor;

#elif defined(BUILD_TARGET_GAME)
	constexpr BuildTarget Target = BuildTarget::Game;

#elif defined(BUILD_TARGET_SERVER)
	constexpr BuildTarget Target = BuildTarget::Server;

#else
#error "Build target is not defined. Please define BUILD_TARGET_* in build settings."
#endif

#if defined(PLATFORM_WINDOWS)
	constexpr BuildPlatform Platform = BuildPlatform::Windows;

#elif defined(PLATFORM_LINUX)
	constexpr BuildPlatform Platform = BuildPlatform::Linux;

#elif defined(PLATFORM_ANDROID)
	constexpr BuildPlatform Platform = BuildPlatform::Android;

#elif defined(PLATFORM_MAC)
	constexpr BuildPlatform Platform = BuildPlatform::Mac;

#else
#error "Platform is not defined. Please include PlatformDefine.h before BuildInfo.h."
#endif

	constexpr bool IsEditorBuild = Target == BuildTarget::Editor;
	constexpr bool IsGameBuild = Target == BuildTarget::Game;
	constexpr bool IsServerBuild = Target == BuildTarget::Server;

	constexpr bool IsWindowsBuild = Platform == BuildPlatform::Windows;
	constexpr bool IsLinuxBuild = Platform == BuildPlatform::Linux;
	constexpr bool IsAndroidBuild = Platform == BuildPlatform::Android;
	constexpr bool IsMacBuild = Platform == BuildPlatform::Mac;
}
