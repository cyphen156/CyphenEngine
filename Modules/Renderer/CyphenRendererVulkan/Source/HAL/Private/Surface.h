#pragma once

#include "HAL/Public/NativeWindowInfo.h"

#include <vulkan/vulkan.h>

// ============================================================================
// Surface
// ----------------------------------------------------------------------------
// CyphenRendererVulkan 모듈 내부의 Platform/HAL 계약입니다.
//
// 책임:
//   - Engine 공통 NativeWindowInfo를 Vulkan VkSurfaceKHR로 변환합니다.
//   - VkInstance 생성 시 필요한 플랫폼 surface extension 이름을 제공합니다.
//   - VulkanRenderer가 Win32 HWND / HINSTANCE 세부 구현을 직접 알지 않게
//     플랫폼별 surface 생성 지점을 숨깁니다.
//
// 비책임:
//   - VkInstance 생성 / 파괴
//   - PhysicalDevice / QueueFamily 선택
//   - Swapchain 생성
//   - RenderCommand 해석
//
// 구현:
//   - Source/Platform/<Target>/Private/Surface.cpp
// ============================================================================

const char* GetVulkanPlatformSurfaceExtensionName();

bool CreateVulkanSurface(
	VkInstance instance,
	const NativeWindowInfo& windowInfo,
	VkSurfaceKHR* outSurface);
