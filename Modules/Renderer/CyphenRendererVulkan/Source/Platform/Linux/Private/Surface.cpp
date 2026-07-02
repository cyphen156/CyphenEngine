#define VK_USE_PLATFORM_XLIB_KHR

#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <cstdint>

#include "HAL/Private/Surface.h"

const char* GetVulkanPlatformSurfaceExtensionName()
{
    return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
}

bool CreateVulkanSurface(
	VkInstance instance,
	void* nativeRenderContextHandle,
	const NativeWindowInfo& windowInfo,
	VkSurfaceKHR* outSurface)
{
	if (instance == VK_NULL_HANDLE ||
		nativeRenderContextHandle == nullptr ||
		windowInfo.nativeWindowHandle == nullptr ||
		outSurface == nullptr)
	{
		return false;
	}

	Display* const display =
		static_cast<Display*>(nativeRenderContextHandle);

	const Window window =
		static_cast<Window>(
			reinterpret_cast<std::uintptr_t>(
				windowInfo.nativeWindowHandle));

	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.dpy = display;
	surfaceCreateInfo.window = window;

	return vkCreateXlibSurfaceKHR(
		instance,
		&surfaceCreateInfo,
		nullptr,
		outSurface) == VK_SUCCESS;
}