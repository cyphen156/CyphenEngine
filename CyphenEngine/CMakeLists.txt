# ============================================================================
# CyphenEngine — CMake 빌드 (실행파일 타깃)
# ----------------------------------------------------------------------------
# 공통 코어 + 실행파일을 CMake로 빌드. MSVC vcxproj와 병존.
#
# 실제 배치:
#   <repo>/
#     BuildArtifacts/
#     CyphenBuild.props
#     CyphenEngine/
#       CMakeLists.txt
#       CyphenEngine.sln
#       CyphenEngine.vcxproj
#       Source/
#       Resources/
#
# MSVC props 기준 산출물 규격:
#   BuildArtifacts/Binaries/<Platform>/<Arch>/<Config>/
#   BuildArtifacts/Intermediate/<Platform>/<Arch>/<Config>/<TargetName>/
#
# WSL2/Linux CMake 사용 예:
#   cd /mnt/c/Project/CyphenEngine/CyphenEngine
#
#   cmake -S . -B ../BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenEngine -G Ninja -DCMAKE_BUILD_TYPE=Debug
#   cmake --build ../BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenEngine
#
# Linux: PlatformFile / PlatformTime / ModuleLoader / main 링크 게이트는
#        2026-06-29 포팅 패스에서 구현한다.
#        실제 Linux 컴파일 검증은 Linux/WSL 툴체인 환경에서 수행한다.
# ============================================================================

cmake_minimum_required(VERSION 3.20)

project(CyphenEngine LANGUAGES CXX)

# ----------------------------------------------------------------------------
# 빌드 타입
# ----------------------------------------------------------------------------
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------
# 공통 소스 (플랫폼 독립)
# ----------------------------------------------------------------------------
set(CYPHEN_COMMON_SOURCES
	Source/Content/Private/Codec.cpp
	Source/Content/Private/Image/ImageCodec.cpp
	Source/Content/Private/Image/Jpeg/JpegCodec.cpp

	Source/Core/Private/File.cpp
	Source/Core/Private/FileStream.cpp
	Source/Core/Private/FileSystem.cpp
	Source/Core/Private/Path.cpp
	Source/Core/Private/Separator.cpp
	Source/Core/Private/TextCodec.cpp
	Source/Core/Private/Thread.cpp
	Source/Core/Private/Time.cpp

	Source/Modules/Private/ModuleBinder.cpp
	Source/Modules/Private/ModuleManager.cpp

	Source/Engine/Private/CyphenEngine.cpp

	Source/Modules/Renderer/Private/Renderer.cpp

	Source/Resource/Private/Texture.cpp

	Source/Test/CoreIO/CoreIOTests.cpp
	Source/Test/Module/ModuleTest.cpp
)

# ----------------------------------------------------------------------------
# 플랫폼 소스
# ----------------------------------------------------------------------------
set(CYPHEN_WINDOWS_SOURCES
	Source/Content/Private/Image/Jpeg/WindowsJpegCodec.cpp
	Source/Platform/Windows/Private/Launch.cpp
	Source/Platform/Windows/Private/ModuleLoader.cpp
	Source/Platform/Windows/Private/PlatformFile.cpp
	Source/Platform/Windows/Private/PlatformPath.cpp
	Source/Platform/Windows/Private/PlatformTime.cpp
	Source/Platform/Windows/Private/WindowsPath.cpp
	Source/Platform/Windows/Private/WindowsString.cpp
	Source/pch.cpp
)

set(CYPHEN_LINUX_SOURCES
	Source/Content/Private/Image/Jpeg/LinuxJpegCodec.cpp
	Source/Platform/Linux/Private/Launch.cpp
	Source/Platform/Linux/Private/LinuxPath.cpp
	Source/Platform/Linux/Private/LinuxString.cpp
	Source/Platform/Linux/Private/ModuleLoader.cpp
	Source/Platform/Linux/Private/PlatformFile.cpp
	Source/Platform/Linux/Private/PlatformPath.cpp
	Source/Platform/Linux/Private/PlatformTime.cpp
	Source/pch.cpp
)

# ----------------------------------------------------------------------------
# 타깃
# ----------------------------------------------------------------------------
if(WIN32)
	enable_language(RC)

	add_executable(CyphenEngine WIN32
		${CYPHEN_COMMON_SOURCES}
		${CYPHEN_WINDOWS_SOURCES}
		Source/Platform/Windows/Resource/CyphenEngine.rc
	)
elseif(UNIX)
	add_executable(CyphenEngine
		${CYPHEN_COMMON_SOURCES}
		${CYPHEN_LINUX_SOURCES}
	)
else()
	message(FATAL_ERROR "Unsupported platform for CyphenEngine.")
endif()

# ----------------------------------------------------------------------------
# 빌드 산출물 경로 규격화
# ----------------------------------------------------------------------------
set(CYPHEN_BUILD_ARTIFACTS_ROOT
	${CMAKE_CURRENT_SOURCE_DIR}/../BuildArtifacts
)

if(WIN32)
	set(CYPHEN_PLATFORM_NAME Windows)
elseif(UNIX)
	set(CYPHEN_PLATFORM_NAME Linux)
else()
	message(FATAL_ERROR "Unsupported platform for CyphenEngine.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
	set(CYPHEN_ARCH_NAME x64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
	set(CYPHEN_ARCH_NAME ARM64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(CYPHEN_ARCH_NAME x64)
else()
	set(CYPHEN_ARCH_NAME x86)
endif()

set(CYPHEN_BINARIES_ROOT
	${CYPHEN_BUILD_ARTIFACTS_ROOT}/Binaries/${CYPHEN_PLATFORM_NAME}/${CYPHEN_ARCH_NAME}
)

foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
	string(TOUPPER ${cfg} CYPHEN_CFG_UPPER)

	set_target_properties(CyphenEngine PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
		ARCHIVE_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
		LIBRARY_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
	)
endforeach()

# ----------------------------------------------------------------------------
# include 루트
#   vcxproj 기준:
#     $(ProjectDir)Source
# ----------------------------------------------------------------------------
target_include_directories(CyphenEngine PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# ----------------------------------------------------------------------------
# 정의
# ----------------------------------------------------------------------------
target_compile_definitions(CyphenEngine PRIVATE
	BUILD_TARGET_EDITOR
	$<$<CONFIG:Debug>:_DEBUG>
	$<$<CONFIG:Release>:NDEBUG>
	$<$<CONFIG:RelWithDebInfo>:NDEBUG>
	$<$<CONFIG:MinSizeRel>:NDEBUG>
)

if(WIN32)
	target_compile_definitions(CyphenEngine PRIVATE
		_WINDOWS
		TARGET_PLATFORM_WINDOWS
		UNICODE
		_UNICODE
	)
elseif(UNIX)
	target_compile_definitions(CyphenEngine PRIVATE
		TARGET_PLATFORM_LINUX
	)
endif()

# ----------------------------------------------------------------------------
# 컴파일 옵션
#   PCH는 CMake target_precompile_headers 를 사용하지 않는다.
#   각 .cpp 첫 줄의 #include "pch.h"는 Source include root로 해석된다.
# ----------------------------------------------------------------------------
if(MSVC)
	target_compile_options(CyphenEngine PRIVATE
		/utf-8
		/permissive-
		/W3
	)
else()
	target_compile_options(CyphenEngine PRIVATE
		-Wall
		-Wextra
	)
endif()

# ----------------------------------------------------------------------------
# 링크
# ----------------------------------------------------------------------------
if(UNIX)
	find_package(Threads REQUIRED)
	find_package(X11 REQUIRED)

	find_path(TURBOJPEG_INCLUDE_DIR NAMES turbojpeg.h)
	find_library(TURBOJPEG_LIBRARY NAMES turbojpeg)

	if(NOT TURBOJPEG_INCLUDE_DIR OR NOT TURBOJPEG_LIBRARY)
		message(FATAL_ERROR "libjpeg-turbo development files are required for Linux JPEG decoding.")
	endif()

	target_include_directories(CyphenEngine PRIVATE
		${TURBOJPEG_INCLUDE_DIR}
		${X11_INCLUDE_DIR}
	)

	target_link_libraries(CyphenEngine PRIVATE
		Threads::Threads
		${TURBOJPEG_LIBRARY}
		${X11_LIBRARIES}
		${CMAKE_DL_LIBS}
	)
endif()

# ----------------------------------------------------------------------------
# CoreIO 테스트 리소스 복사
# ----------------------------------------------------------------------------
add_custom_command(TARGET CyphenEngine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Resources/Test/CoreIo
		$<TARGET_FILE_DIR:CyphenEngine>/Resources/Test/CoreIo
)
