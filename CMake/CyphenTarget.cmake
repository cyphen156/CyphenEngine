# ============================================================================
# CyphenTarget
# ----------------------------------------------------------------------------
# CyphenEngine CMake target 공통 규칙입니다.
#
# 책임:
#   - BuildArtifacts 산출물 경로 규격화
#   - TARGET_PLATFORM_* 주입
#   - BUILD_TARGET_* 주입
#   - 공통 C++ 옵션 적용
#   - strict C++17 적용
#   - Linux runtime module lookup용 RPATH 적용
#   - Module shared library용 visibility / 파일명 규칙 적용
# ============================================================================

if(NOT DEFINED CYPHEN_REPO_ROOT)
	message(FATAL_ERROR "CYPHEN_REPO_ROOT must be defined before including CyphenTarget.cmake.")
endif()

if(NOT DEFINED CYPHEN_BUILD_TARGET)
	set(CYPHEN_BUILD_TARGET Editor CACHE STRING "Cyphen build target.")
	set_property(CACHE CYPHEN_BUILD_TARGET PROPERTY STRINGS Editor Game Server)
endif()

function(cyphen_get_platform_name out_var)
	if(WIN32)
		set(${out_var} Windows PARENT_SCOPE)
	elseif(UNIX AND NOT APPLE)
		set(${out_var} Linux PARENT_SCOPE)
	elseif(APPLE)
		set(${out_var} Mac PARENT_SCOPE)
	else()
		message(FATAL_ERROR "Unsupported platform.")
	endif()
endfunction()

function(cyphen_get_arch_name out_var)
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
		set(${out_var} x64 PARENT_SCOPE)
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
		set(${out_var} ARM64 PARENT_SCOPE)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(${out_var} x64 PARENT_SCOPE)
	else()
		set(${out_var} x86 PARENT_SCOPE)
	endif()
endfunction()

function(cyphen_apply_output_layout target_name)
	cyphen_get_platform_name(CYPHEN_PLATFORM_NAME)
	cyphen_get_arch_name(CYPHEN_ARCH_NAME)

	set(CYPHEN_BINARIES_ROOT
		${CYPHEN_REPO_ROOT}/BuildArtifacts/Binaries/${CYPHEN_PLATFORM_NAME}/${CYPHEN_ARCH_NAME}
	)

	foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
		string(TOUPPER ${cfg} CYPHEN_CFG_UPPER)

		set_target_properties(${target_name} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
			ARCHIVE_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
			LIBRARY_OUTPUT_DIRECTORY_${CYPHEN_CFG_UPPER} ${CYPHEN_BINARIES_ROOT}/${cfg}
		)
	endforeach()
endfunction()

function(cyphen_apply_platform_defines target_name)
	if(WIN32)
		target_compile_definitions(${target_name} PRIVATE
			TARGET_PLATFORM_WINDOWS
			_WINDOWS
			UNICODE
			_UNICODE
		)
	elseif(UNIX AND NOT APPLE)
		target_compile_definitions(${target_name} PRIVATE
			TARGET_PLATFORM_LINUX
		)
	elseif(APPLE)
		target_compile_definitions(${target_name} PRIVATE
			TARGET_PLATFORM_MAC
		)
	endif()
endfunction()

function(cyphen_apply_build_target target_name)
	if(CYPHEN_BUILD_TARGET STREQUAL "Editor")
		target_compile_definitions(${target_name} PRIVATE BUILD_TARGET_EDITOR)
	elseif(CYPHEN_BUILD_TARGET STREQUAL "Game")
		target_compile_definitions(${target_name} PRIVATE BUILD_TARGET_GAME)
	elseif(CYPHEN_BUILD_TARGET STREQUAL "Server")
		target_compile_definitions(${target_name} PRIVATE BUILD_TARGET_SERVER)
	else()
		message(FATAL_ERROR "Unsupported CYPHEN_BUILD_TARGET: ${CYPHEN_BUILD_TARGET}")
	endif()
endfunction()

function(cyphen_apply_config_defines target_name)
	target_compile_definitions(${target_name} PRIVATE
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:RelWithDebInfo>:NDEBUG>
		$<$<CONFIG:MinSizeRel>:NDEBUG>
	)
endfunction()

function(cyphen_apply_common_cpp target_name)
	target_compile_features(${target_name} PRIVATE cxx_std_17)

	set_target_properties(${target_name} PROPERTIES
		CXX_EXTENSIONS OFF
	)

	cyphen_apply_config_defines(${target_name})

	if(MSVC)
		target_compile_options(${target_name} PRIVATE
			/utf-8
			/permissive-
			/W3
		)
	else()
		target_compile_options(${target_name} PRIVATE
			-Wall
			-Wextra
		)
	endif()
endfunction()

function(cyphen_apply_runtime_link_path target_name)
	if(UNIX AND NOT APPLE)
		set_target_properties(${target_name} PROPERTIES
			BUILD_RPATH "$ORIGIN"
			INSTALL_RPATH "$ORIGIN"
		)
	endif()
endfunction()

function(cyphen_configure_target target_name)
	cyphen_apply_common_cpp(${target_name})
	cyphen_apply_platform_defines(${target_name})
	cyphen_apply_build_target(${target_name})
	cyphen_apply_output_layout(${target_name})
	cyphen_apply_runtime_link_path(${target_name})
endfunction()

function(cyphen_configure_module target_name)
	cyphen_configure_target(${target_name})

	set_target_properties(${target_name} PROPERTIES
		PREFIX ""
		OUTPUT_NAME "${target_name}"
	)

	target_include_directories(${target_name} PRIVATE
		${CYPHEN_REPO_ROOT}/CyphenEngine/Source
	)

	if(NOT MSVC)
		target_compile_options(${target_name} PRIVATE
			-fvisibility=hidden
		)
	endif()
endfunction()

function(cyphen_add_module target_name)
	add_library(${target_name} SHARED ${ARGN})

	cyphen_configure_module(${target_name})

	target_include_directories(${target_name} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Source
	)
endfunction()

function(cyphen_add_module_subdirectory module_dir)
	add_subdirectory(${module_dir})
endfunction()

function(cyphen_add_client_module_subdirectory module_dir)
	if(CYPHEN_BUILD_TARGET STREQUAL "Server")
		return()
	endif()

	add_subdirectory(${module_dir})
endfunction()