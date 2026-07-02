# ============================================================================
# CyphenRendererVulkan - CMake build
# ----------------------------------------------------------------------------
# Vulkan renderer backend shared module.
# ============================================================================

cmake_minimum_required(VERSION 3.20)

project(CyphenRendererVulkan LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(NOT DEFINED CYPHEN_REPO_ROOT)
	set(CYPHEN_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
endif()

include(${CYPHEN_REPO_ROOT}/CMake/CyphenTarget.cmake)

cyphen_add_module(CyphenRendererVulkan
	Source/Private/VulkanRenderer.cpp
	Source/Private/VulkanRendererModule.cpp

	Source/HAL/Private/Surface.h

	$<$<PLATFORM_ID:Windows>:Source/Platform/Windows/Private/Surface.cpp>
	$<$<PLATFORM_ID:Linux>:Source/Platform/Linux/Private/Surface.cpp>
)

find_package(Vulkan REQUIRED)

target_link_libraries(CyphenRendererVulkan PRIVATE
	Vulkan::Vulkan
)

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)

	target_include_directories(CyphenRendererVulkan PRIVATE
		${X11_INCLUDE_DIR}
	)

	target_link_libraries(CyphenRendererVulkan PRIVATE
		${X11_LIBRARIES}
	)
endif()

find_program(GLSLANG_VALIDATOR glslangValidator)

if(NOT GLSLANG_VALIDATOR)
	message(FATAL_ERROR "glslangValidator is required to build CyphenRendererVulkan shaders.")
endif()

add_custom_command(TARGET CyphenRendererVulkan POST_BUILD
	COMMAND ${GLSLANG_VALIDATOR}
		-V
		${CMAKE_CURRENT_SOURCE_DIR}/Source/Private/TexturedQuad.vert
		-o
		$<TARGET_FILE_DIR:CyphenRendererVulkan>/TexturedQuad.vert.spv
	COMMAND ${GLSLANG_VALIDATOR}
		-V
		${CMAKE_CURRENT_SOURCE_DIR}/Source/Private/TexturedQuad.frag
		-o
		$<TARGET_FILE_DIR:CyphenRendererVulkan>/TexturedQuad.frag.spv
	COMMENT "Compiling CyphenRendererVulkan shaders"
	VERBATIM
)