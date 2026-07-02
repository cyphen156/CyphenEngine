# ============================================================================
# CyphenEngine - CMake build
# ----------------------------------------------------------------------------
# Engine executable target.
#
# WSL2/Linux CMake 사용 예:
#   cd /mnt/c/Project/CyphenEngine
#   cmake -S . -B BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenWorkspace -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCYPHEN_BUILD_TARGET=Editor
#   cmake --build BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenWorkspace
# ============================================================================

cmake_minimum_required(VERSION 3.20)

project(CyphenEngine LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(NOT DEFINED CYPHEN_REPO_ROOT)
	set(CYPHEN_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
endif()

include(${CYPHEN_REPO_ROOT}/CMake/CyphenTarget.cmake)

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

if(WIN32)
	enable_language(RC)

	add_executable(CyphenEngine WIN32
		${CYPHEN_COMMON_SOURCES}
		${CYPHEN_WINDOWS_SOURCES}
		Source/Platform/Windows/Resource/CyphenEngine.rc
	)
elseif(UNIX AND NOT APPLE)
	add_executable(CyphenEngine
		${CYPHEN_COMMON_SOURCES}
		${CYPHEN_LINUX_SOURCES}
	)
else()
	message(FATAL_ERROR "Unsupported platform for CyphenEngine.")
endif()

cyphen_configure_target(CyphenEngine)

target_include_directories(CyphenEngine PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Source
)

if(UNIX AND NOT APPLE)
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

add_custom_command(TARGET CyphenEngine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Resources
		$<TARGET_FILE_DIR:CyphenEngine>/Resources
)
