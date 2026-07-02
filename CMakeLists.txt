# ============================================================================
# CyphenWorkspace - CMake build
# ----------------------------------------------------------------------------
# Engine executable과 backend module을 같은 configure/build 안에서 묶습니다.
#
# 이유:
#   - Engine/Module의 CMAKE_BUILD_TYPE 불일치 방지
#   - Engine/Module의 CYPHEN_BUILD_TARGET 불일치 방지
#   - RendererModuleApi debug/release ABI 차이로 인한 런타임 함수표 파손 방지
#
# 사용 예:
#   cmake -S . -B BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenWorkspace -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCYPHEN_BUILD_TARGET=Editor
#   cmake --build BuildArtifacts/Intermediate/Linux/x64/Debug/CyphenWorkspace
# ============================================================================

cmake_minimum_required(VERSION 3.20)

project(CyphenWorkspace LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CYPHEN_REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CYPHEN_BUILD_TARGET Editor CACHE STRING "Cyphen build target.")
set_property(CACHE CYPHEN_BUILD_TARGET PROPERTY STRINGS Editor Game Server)

include(${CYPHEN_REPO_ROOT}/CMake/CyphenTarget.cmake)

add_subdirectory(CyphenEngine)

cyphen_add_client_module_subdirectory(Modules/Renderer/CyphenRendererVulkan)