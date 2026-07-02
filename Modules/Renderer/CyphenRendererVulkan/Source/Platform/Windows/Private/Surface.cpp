#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>

#include <windows.h>

#include "HAL/Private/Surface.h"

const char* GetVulkanPlatformSurfaceExtensionName()
{
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
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
	
	const HINSTANCE instanceHandle = static_cast<HINSTANCE>(nativeRenderContextHandle);

	const HWND windowHandle = static_cast<HWND>(windowInfo.nativeWindowHandle);

	if (::IsWindow(windowHandle) == FALSE)
	{
		return false;
	}

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = instanceHandle;
	surfaceCreateInfo.hwnd = windowHandle;

	return vkCreateWin32SurfaceKHR(
		instance,
		&surfaceCreateInfo,
		nullptr,
		outSurface) == VK_SUCCESS;
}
