#define VK_USE_PLATFORM_UXIX_KHR

#include <vulkan/vulkan.h>

#include "HAL/Private/Surface.h"

bool CreateVulkanSurface(
	VkInstance instance,
	const NativeWindowInfo& windowInfo,
	VkSurfaceKHR& outSurface)
{
}