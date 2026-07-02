#include <cstring>
#include <fstream>
#include <limits>

#ifdef _DEBUG
#include "Build/Public/Framework.h"
#endif

#include "VulkanRenderer.h"
#include "HAL/Private/Surface.h"

namespace
{
	const char* RequiredDeviceExtensions[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool IsSuccess(VkResult result)
	{
		return result == VK_SUCCESS;
	}

	VkExtent2D ClampExtent(const VkSurfaceCapabilitiesKHR& capabilities, const NativeWindowInfo& windowInfo)
	{
		if (capabilities.currentExtent.width != (std::numeric_limits<uint32>::max)())
		{
			return capabilities.currentExtent;
		}

		VkExtent2D extent = {};
		extent.width = windowInfo.windowWidth;
		extent.height = windowInfo.windowHeight;

		if (extent.width < capabilities.minImageExtent.width)
		{
			extent.width = capabilities.minImageExtent.width;
		}

		if (extent.width > capabilities.maxImageExtent.width)
		{
			extent.width = capabilities.maxImageExtent.width;
		}

		if (extent.height < capabilities.minImageExtent.height)
		{
			extent.height = capabilities.minImageExtent.height;
		}

		if (extent.height > capabilities.maxImageExtent.height)
		{
			extent.height = capabilities.maxImageExtent.height;
		}

		return extent;
	}

	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		for (const VkSurfaceFormatKHR& format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return formats[0];
	}
}

bool VulkanQueueFamilySelection::IsComplete() const
{
	return graphicsFamily != UINT32_MAX &&
		presentFamily != UINT32_MAX;
}

bool VulkanRenderer::Initialize(void* nativeRenderContextHandle, const NativeWindowInfo& windowInfo)
{
	if (windowInfo.nativeWindowHandle == nullptr ||
		windowInfo.windowWidth == 0 ||
		windowInfo.windowHeight == 0)
	{
		return false;
	}

#ifdef _DEBUG
	PRINT_DEBUG_OUTPUT("[VulkanRenderer] Initialize begin\n");

	if (CreateInstance() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateInstance failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateInstance success\n");

	if (CreateSurface(nativeRenderContextHandle, windowInfo) == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSurface failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSurface success\n");

	if (SelectPhysicalDevice() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] SelectPhysicalDevice failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] SelectPhysicalDevice success\n");

	if (CreateDevice() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateDevice failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateDevice success\n");

	if (CreateSwapchain(windowInfo) == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSwapchain failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSwapchain success\n");

	if (CreateRenderPass() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateRenderPass failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateRenderPass success\n");

	if (CreateDescriptorObjects() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateDescriptorObjects failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateDescriptorObjects success\n");

	if (CreatePipeline() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreatePipeline failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreatePipeline success\n");

	if (CreateFramebuffers() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateFramebuffers failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateFramebuffers success\n");

	if (CreateCommandPool() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateCommandPool failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateCommandPool success\n");

	if (CreateSyncObjects() == false)
	{
		PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSyncObjects failed\n");
		Shutdown();

		return false;
	}

	PRINT_DEBUG_OUTPUT("[VulkanRenderer] CreateSyncObjects success\n");
	PRINT_DEBUG_OUTPUT("[VulkanRenderer] Initialize success\n");
#else
	if (CreateInstance() == false || 
		CreateSurface(nativeRenderContextHandle, windowInfo) == false ||
		SelectPhysicalDevice() == false ||
		CreateDevice() == false || 
		CreateSwapchain(windowInfo) == false || 
		CreateRenderPass() == false || 
		CreateDescriptorObjects() == false || 
		CreatePipeline() == false || 
		CreateFramebuffers() == false || 
		CreateCommandPool() == false || 
		CreateSyncObjects() == false) 
	{ 
		Shutdown(); 
		return false; 
	}
#endif // DEBUG

	return true;
}

void VulkanRenderer::Shutdown()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(device);
	}

#ifdef _DEBUG
	for (VulkanTexture2D& texture : texture2DTable)
	{
		ReleaseTexture(texture);
	}

	texture2DTable.clear();
#endif

	if (inFlightFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device, inFlightFence, nullptr);
		inFlightFence = VK_NULL_HANDLE;
	}

	if (renderFinishedSemaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		renderFinishedSemaphore = VK_NULL_HANDLE;
	}

	if (imageAvailableSemaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		imageAvailableSemaphore = VK_NULL_HANDLE;
	}

	if (commandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;
	}

	ReleasePipelineObjects();
	ReleaseSwapchainObjects();

	if (sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(device, sampler, nullptr);
		sampler = VK_NULL_HANDLE;
	}

	if (descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}

	if (descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
	}

	if (device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}

	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}

	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}

	physicalDevice = VK_NULL_HANDLE;
	queueFamilies = {};
	graphicsQueue = VK_NULL_HANDLE;
	presentQueue = VK_NULL_HANDLE;
	pendingTextureId = InvalidResourceId;
}

bool VulkanRenderer::ExecuteCommandList(const RenderCommandList& commandList)
{
	if (commandList.words == nullptr ||
		commandList.wordCount == 0 ||
		commandList.commandCount == 0)
	{
		return false;
	}

	uint32 cursor = 0;
	uint32 executedCommandCount = 0;
	pendingTextureId = InvalidResourceId;

	while (cursor < commandList.wordCount)
	{
		RenderCommandHeader header = {};
		std::memcpy(&header, &commandList.words[cursor], sizeof(header));
		++cursor;

		if (header.payloadWordCount > commandList.wordCount - cursor)
		{
			return false;
		}

		const RenderCommandWord* payloadWords = &commandList.words[cursor];
		const RenderCommandType commandType =
			static_cast<RenderCommandType>(header.commandType);

		switch (commandType)
		{
		case RenderCommandType::ClearRenderTarget:
			if (ExecuteClearRenderTarget(payloadWords, header.payloadWordCount) == false)
			{
				return false;
			}
			break;

		case RenderCommandType::DrawTexturedQuad:
			if (ExecuteDrawTexturedQuad(payloadWords, header.payloadWordCount) == false)
			{
				return false;
			}
			break;

		case RenderCommandType::Present:
			if (ExecutePresent(header.payloadWordCount) == false)
			{
				return false;
			}
			break;

		default:
			return false;
		}

		cursor += header.payloadWordCount;
		++executedCommandCount;
	}

	return cursor == commandList.wordCount &&
		executedCommandCount == commandList.commandCount;
}

#ifdef _DEBUG
bool VulkanRenderer::ExecuteResourceCommandList(const ResourceCommandList& commandList)
{
	if (commandList.words == nullptr ||
		commandList.wordCount == 0 ||
		commandList.commandCount == 0)
	{
		return false;
	}

	uint32 cursor = 0;
	uint32 executedCommandCount = 0;

	while (cursor < commandList.wordCount)
	{
		ResourceCommandHeader header = {};
		std::memcpy(&header, &commandList.words[cursor], sizeof(header));
		++cursor;

		if (header.payloadWordCount > commandList.wordCount - cursor)
		{
			return false;
		}

		const ResourceCommandWord* payloadWords = &commandList.words[cursor];
		const ResourceCommandType commandType =
			static_cast<ResourceCommandType>(header.commandType);

		switch (commandType)
		{
		case ResourceCommandType::UploadResource:
			if (ExecuteUploadResource(payloadWords, header.payloadWordCount) == false)
			{
				return false;
			}
			break;

		case ResourceCommandType::DestroyResource:
			if (ExecuteDestroyResource(payloadWords, header.payloadWordCount) == false)
			{
				return false;
			}
			break;

		default:
			return false;
		}

		cursor += header.payloadWordCount;
		++executedCommandCount;
	}

	return cursor == commandList.wordCount &&
		executedCommandCount == commandList.commandCount;
}
#endif

bool VulkanRenderer::CreateInstance()
{
	const char* extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		GetVulkanPlatformSurfaceExtensionName()
	};

	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "CyphenEngine";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.pEngineName = "CyphenEngine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledExtensionCount = static_cast<uint32>(sizeof(extensions) / sizeof(extensions[0]));
	createInfo.ppEnabledExtensionNames = extensions;

	return IsSuccess(vkCreateInstance(&createInfo, nullptr, &instance)) &&
		instance != VK_NULL_HANDLE;
}

bool VulkanRenderer::CreateSurface(void* nativeRenderContextHandle, const NativeWindowInfo& windowInfo)
{
	return CreateVulkanSurface(
		instance,
		nativeRenderContextHandle,
		windowInfo,
		&surface);
}

bool VulkanRenderer::SelectPhysicalDevice()
{
	uint32 deviceCount = 0;

	if (IsSuccess(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) == false ||
		deviceCount == 0)
	{
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);

	if (IsSuccess(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data())) == false)
	{
		return false;
	}

	for (VkPhysicalDevice candidate : devices)
	{
		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(candidate, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			candidate,
			&queueFamilyCount,
			queueFamiliesProperties.data());

		VulkanQueueFamilySelection selection = {};

		for (uint32 index = 0; index < queueFamilyCount; ++index)
		{
			if ((queueFamiliesProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				selection.graphicsFamily = index;
			}

			VkBool32 supportsPresent = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(candidate, index, surface, &supportsPresent);

			if (supportsPresent == VK_TRUE)
			{
				selection.presentFamily = index;
			}
		}

		uint32 extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(candidate, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensionProperties(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			candidate,
			nullptr,
			&extensionCount,
			extensionProperties.data());

		bool hasSwapchainExtension = false;

		for (const VkExtensionProperties& extensionProperty : extensionProperties)
		{
			if (std::strcmp(extensionProperty.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			{
				hasSwapchainExtension = true;
				break;
			}
		}

		uint32 formatCount = 0;
		uint32 presentModeCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(candidate, surface, &formatCount, nullptr);
		vkGetPhysicalDeviceSurfacePresentModesKHR(candidate, surface, &presentModeCount, nullptr);

		if (selection.IsComplete() &&
			hasSwapchainExtension &&
			formatCount > 0 &&
			presentModeCount > 0)
		{
			physicalDevice = candidate;
			queueFamilies = selection;
			return true;
		}
	}

	return false;
}

bool VulkanRenderer::CreateDevice()
{
	const float queuePriority = 1.0f;
	std::array<uint32, 2> familyIndices =
	{
		queueFamilies.graphicsFamily,
		queueFamilies.presentFamily
	};

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	for (uint32 familyIndex : familyIndices)
	{
		bool alreadyAdded = false;

		for (const VkDeviceQueueCreateInfo& queueCreateInfo : queueCreateInfos)
		{
			if (queueCreateInfo.queueFamilyIndex == familyIndex)
			{
				alreadyAdded = true;
				break;
			}
		}

		if (alreadyAdded)
		{
			continue;
		}

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = familyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount =
		static_cast<uint32>(sizeof(RequiredDeviceExtensions) / sizeof(RequiredDeviceExtensions[0]));
	createInfo.ppEnabledExtensionNames = RequiredDeviceExtensions;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (IsSuccess(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)) == false)
	{
		return false;
	}

	vkGetDeviceQueue(device, queueFamilies.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, queueFamilies.presentFamily, 0, &presentQueue);

	return graphicsQueue != VK_NULL_HANDLE &&
		presentQueue != VK_NULL_HANDLE;
}

bool VulkanRenderer::CreateSwapchain(const NativeWindowInfo& windowInfo)
{
	VkSurfaceCapabilitiesKHR capabilities = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	uint32 formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

	const VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
	const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	const VkExtent2D extent = ClampExtent(capabilities, windowInfo);

	uint32 imageCount = capabilities.minImageCount + 1;

	if (capabilities.maxImageCount > 0 &&
		imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32 queueFamilyIndices[] =
	{
		queueFamilies.graphicsFamily,
		queueFamilies.presentFamily
	};

	if (queueFamilies.graphicsFamily != queueFamilies.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	if (IsSuccess(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain)) == false)
	{
		return false;
	}

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	swapchainImageViews.resize(swapchainImages.size());

	for (size_t index = 0; index < swapchainImages.size(); ++index)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = swapchainImages[index];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = swapchainImageFormat;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (IsSuccess(vkCreateImageView(
			device,
			&imageViewCreateInfo,
			nullptr,
			&swapchainImageViews[index])) == false)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	return IsSuccess(vkCreateRenderPass(device, &createInfo, nullptr, &renderPass));
}

bool VulkanRenderer::CreateFramebuffers()
{
	framebuffers.resize(swapchainImageViews.size());

	for (size_t index = 0; index < swapchainImageViews.size(); ++index)
	{
		VkImageView attachments[] =
		{
			swapchainImageViews[index]
		};

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.width = swapchainExtent.width;
		createInfo.height = swapchainExtent.height;
		createInfo.layers = 1;

		if (IsSuccess(vkCreateFramebuffer(
			device,
			&createInfo,
			nullptr,
			&framebuffers[index])) == false)
		{
			return false;
		}
	}

	commandBuffers.resize(framebuffers.size());

	return true;
}

bool VulkanRenderer::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = queueFamilies.graphicsFamily;

	if (IsSuccess(vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool)) == false)
	{
		return false;
	}

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = static_cast<uint32>(commandBuffers.size());

	return IsSuccess(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()));
}

bool VulkanRenderer::CreateDescriptorObjects()
{
	VkDescriptorSetLayoutBinding textureBinding = {};
	textureBinding.binding = 0;
	textureBinding.descriptorCount = 1;
	textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &textureBinding;

	if (IsSuccess(vkCreateDescriptorSetLayout(
		device,
		&layoutCreateInfo,
		nullptr,
		&descriptorSetLayout)) == false)
	{
		return false;
	}

	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = 64;

	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = 64;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;

	if (IsSuccess(vkCreateDescriptorPool(
		device,
		&poolCreateInfo,
		nullptr,
		&descriptorPool)) == false)
	{
		return false;
	}

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	return IsSuccess(vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler));
}

bool VulkanRenderer::CreatePipeline()
{
	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	if (LoadShaderModule("TexturedQuad.vert.spv", &vertexShaderModule) == false ||
		LoadShaderModule("TexturedQuad.frag.spv", &fragmentShaderModule) == false)
	{
		if (vertexShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, vertexShaderModule, nullptr);
		}

		if (fragmentShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
		}

		return false;
	}

	VkPipelineShaderStageCreateInfo shaderStages[2] = {};
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShaderModule;
	shaderStages[0].pName = "main";
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragmentShaderModule;
	shaderStages[1].pName = "main";

	VkPipelineVertexInputStateCreateInfo vertexInput = {};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.extent = swapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	if (IsSuccess(vkCreatePipelineLayout(
		device,
		&pipelineLayoutCreateInfo,
		nullptr,
		&pipelineLayout)) == false)
	{
		vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
		vkDestroyShaderModule(device, vertexShaderModule, nullptr);
		return false;
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.pVertexInputState = &vertexInput;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0;

	const bool result = IsSuccess(vkCreateGraphicsPipelines(
		device,
		VK_NULL_HANDLE,
		1,
		&pipelineCreateInfo,
		nullptr,
		&pipeline));

	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);

	return result;
}

bool VulkanRenderer::CreateSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	return IsSuccess(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore)) &&
		IsSuccess(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore)) &&
		IsSuccess(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFence));
}

bool VulkanRenderer::ExecuteClearRenderTarget(
	const RenderCommandWord* payloadWords,
	uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount != 2)
	{
		return false;
	}

	ClearRenderTargetCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));
	std::memcpy(pendingClearColor, command.color, sizeof(pendingClearColor));

	return true;
}

bool VulkanRenderer::ExecuteDrawTexturedQuad(const RenderCommandWord* payloadWords, uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount != 1)
	{
		return false;
	}

	DrawTexturedQuadCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));

	if (FindTexture2D(command.textureId) == nullptr)
	{
		return false;
	}

	pendingTextureId = command.textureId;

	return true;
}

bool VulkanRenderer::ExecutePresent(uint32 payloadWordCount)
{
	return payloadWordCount == 0 &&
		RenderAndPresent();
}

bool VulkanRenderer::RenderAndPresent()
{
	if (device == VK_NULL_HANDLE ||
		swapchain == VK_NULL_HANDLE ||
		commandBuffers.empty())
	{
		return false;
	}

	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

	uint32 imageIndex = 0;
	VkResult acquireResult = vkAcquireNextImageKHR(
		device,
		swapchain,
		UINT64_MAX,
		imageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	if (acquireResult != VK_SUCCESS &&
		acquireResult != VK_SUBOPTIMAL_KHR)
	{
		return false;
	}

	vkResetFences(device, 1, &inFlightFence);

	VkCommandBuffer commandBuffer = commandBuffers[imageIndex];
	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (IsSuccess(vkBeginCommandBuffer(commandBuffer, &beginInfo)) == false)
	{
		return false;
	}

	VkClearValue clearValue = {};
	clearValue.color.float32[0] = pendingClearColor[0];
	clearValue.color.float32[1] = pendingClearColor[1];
	clearValue.color.float32[2] = pendingClearColor[2];
	clearValue.color.float32[3] = pendingClearColor[3];

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffers[imageIndex];
	renderPassBeginInfo.renderArea.extent = swapchainExtent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VulkanTexture2D* texture = FindTexture2D(pendingTextureId);

	if (texture != nullptr)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&texture->descriptorSet,
			0,
			nullptr);
		vkCmdDraw(commandBuffer, 6, 1, 0, 0);
	}

	vkCmdEndRenderPass(commandBuffer);

	if (IsSuccess(vkEndCommandBuffer(commandBuffer)) == false)
	{
		return false;
	}

	VkPipelineStageFlags waitStages[] =
	{
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

	if (IsSuccess(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence)) == false)
	{
		return false;
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;

	VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

	return presentResult == VK_SUCCESS ||
		presentResult == VK_SUBOPTIMAL_KHR;
}

#ifdef _DEBUG
bool VulkanRenderer::ExecuteUploadResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount == 0)
	{
		return false;
	}

	const uint64 availablePayloadBytes =
		static_cast<uint64>(payloadWordCount) *
		static_cast<uint64>(sizeof(ResourceCommandWord));

	if (availablePayloadBytes < sizeof(UploadResourceCommand))
	{
		return false;
	}

	UploadResourceCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));

	if (command.resourceId == InvalidResourceId ||
		command.resourceKind != ResourceKind::Texture2D ||
		command.payloadByteCount < sizeof(Texture2DUploadPayload) ||
		sizeof(command) + command.payloadByteCount > availablePayloadBytes)
	{
		return false;
	}

	const uint8* payloadBytes =
		reinterpret_cast<const uint8*>(payloadWords) +
		sizeof(command);

	return UploadTexture2D(command, payloadBytes, command.payloadByteCount);
}

bool VulkanRenderer::UploadTexture2D(
	const UploadResourceCommand& command,
	const uint8* payloadBytes,
	uint64 payloadByteCount)
{
	if (device == VK_NULL_HANDLE ||
		payloadBytes == nullptr ||
		payloadByteCount < sizeof(Texture2DUploadPayload))
	{
		return false;
	}

	Texture2DUploadPayload texturePayload = {};
	std::memcpy(&texturePayload, payloadBytes, sizeof(texturePayload));

	if (texturePayload.format != TextureFormat::Rgba8 ||
		texturePayload.width == 0 ||
		texturePayload.height == 0 ||
		command.resourceId > static_cast<ResourceId>((std::numeric_limits<uint32>::max)()))
	{
		return false;
	}

	const VkDeviceSize pixelByteCount =
		static_cast<VkDeviceSize>(payloadByteCount - sizeof(texturePayload));
	const VkDeviceSize expectedPixelByteCount =
		static_cast<VkDeviceSize>(texturePayload.width) *
		static_cast<VkDeviceSize>(texturePayload.height) *
		4;

	if (pixelByteCount != expectedPixelByteCount)
	{
		return false;
	}

	const uint8* pixels = payloadBytes + sizeof(texturePayload);

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

	if (CreateBuffer(
		pixelByteCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer,
		&stagingMemory) == false)
	{
		return false;
	}

	void* mappedMemory = nullptr;

	if (IsSuccess(vkMapMemory(device, stagingMemory, 0, pixelByteCount, 0, &mappedMemory)) == false)
	{
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return false;
	}

	std::memcpy(mappedMemory, pixels, static_cast<size_t>(pixelByteCount));
	vkUnmapMemory(device, stagingMemory);

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = texturePayload.width;
	imageCreateInfo.extent.height = texturePayload.height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VulkanTexture2D createdTexture = {};
	createdTexture.resourceId = command.resourceId;
	createdTexture.width = texturePayload.width;
	createdTexture.height = texturePayload.height;

	if (IsSuccess(vkCreateImage(device, &imageCreateInfo, nullptr, &createdTexture.image)) == false)
	{
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return false;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(device, createdTexture.image, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemoryType(
		memoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (IsSuccess(vkAllocateMemory(device, &allocateInfo, nullptr, &createdTexture.memory)) == false ||
		IsSuccess(vkBindImageMemory(device, createdTexture.image, createdTexture.memory, 0)) == false)
	{
		ReleaseTexture(createdTexture);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return false;
	}

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	if (BeginImmediateCommands(&commandBuffer) == false)
	{
		ReleaseTexture(createdTexture);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return false;
	}

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = createdTexture.image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.layerCount = 1;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	VkBufferImageCopy copyRegion = {};
	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageExtent.width = texturePayload.width;
	copyRegion.imageExtent.height = texturePayload.height;
	copyRegion.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		commandBuffer,
		stagingBuffer,
		createdTexture.image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copyRegion);

	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	if (EndImmediateCommands(commandBuffer) == false)
	{
		ReleaseTexture(createdTexture);
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
		return false;
	}

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = createdTexture.image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	if (IsSuccess(vkCreateImageView(
		device,
		&imageViewCreateInfo,
		nullptr,
		&createdTexture.imageView)) == false)
	{
		ReleaseTexture(createdTexture);
		return false;
	}

	VkDescriptorSetAllocateInfo descriptorAllocateInfo = {};
	descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorAllocateInfo.descriptorPool = descriptorPool;
	descriptorAllocateInfo.descriptorSetCount = 1;
	descriptorAllocateInfo.pSetLayouts = &descriptorSetLayout;

	if (IsSuccess(vkAllocateDescriptorSets(
		device,
		&descriptorAllocateInfo,
		&createdTexture.descriptorSet)) == false)
	{
		ReleaseTexture(createdTexture);
		return false;
	}

	VkDescriptorImageInfo descriptorImageInfo = {};
	descriptorImageInfo.sampler = sampler;
	descriptorImageInfo.imageView = createdTexture.imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = createdTexture.descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.pImageInfo = &descriptorImageInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

	const size_t tableIndex = static_cast<size_t>(command.resourceId);

	if (texture2DTable.size() <= tableIndex)
	{
		texture2DTable.resize(tableIndex + 1);
	}

	ReleaseTexture(texture2DTable[tableIndex]);
	texture2DTable[tableIndex] = createdTexture;

	return true;
}

bool VulkanRenderer::ExecuteDestroyResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount != 1)
	{
		return false;
	}

	DestroyResourceCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));

	if (command.resourceId == InvalidResourceId ||
		command.resourceId > static_cast<ResourceId>((std::numeric_limits<uint32>::max)()))
	{
		return false;
	}

	const size_t tableIndex = static_cast<size_t>(command.resourceId);

	if (tableIndex < texture2DTable.size())
	{
		ReleaseTexture(texture2DTable[tableIndex]);
		texture2DTable[tableIndex] = {};
	}

	return true;
}
#endif

VulkanTexture2D* VulkanRenderer::FindTexture2D(ResourceId resourceId)
{
#ifdef _DEBUG
	if (resourceId == InvalidResourceId ||
		resourceId > static_cast<ResourceId>((std::numeric_limits<uint32>::max)()))
	{
		return nullptr;
	}

	const size_t tableIndex = static_cast<size_t>(resourceId);

	if (tableIndex >= texture2DTable.size())
	{
		return nullptr;
	}

	VulkanTexture2D& texture = texture2DTable[tableIndex];

	if (texture.resourceId != resourceId ||
		texture.imageView == VK_NULL_HANDLE ||
		texture.descriptorSet == VK_NULL_HANDLE)
	{
		return nullptr;
	}

	return &texture;
#else
	(void)resourceId;
	return nullptr;
#endif
}

uint32 VulkanRenderer::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32 index = 0; index < memoryProperties.memoryTypeCount; ++index)
	{
		if ((typeFilter & (1u << index)) != 0 &&
			(memoryProperties.memoryTypes[index].propertyFlags & properties) == properties)
		{
			return index;
		}
	}

	return UINT32_MAX;
}

bool VulkanRenderer::CreateBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer* outBuffer,
	VkDeviceMemory* outMemory)
{
	if (outBuffer == nullptr ||
		outMemory == nullptr)
	{
		return false;
	}

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (IsSuccess(vkCreateBuffer(device, &bufferCreateInfo, nullptr, outBuffer)) == false)
	{
		return false;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(device, *outBuffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (allocateInfo.memoryTypeIndex == UINT32_MAX ||
		IsSuccess(vkAllocateMemory(device, &allocateInfo, nullptr, outMemory)) == false)
	{
		vkDestroyBuffer(device, *outBuffer, nullptr);
		*outBuffer = VK_NULL_HANDLE;
		return false;
	}

	if (IsSuccess(vkBindBufferMemory(device, *outBuffer, *outMemory, 0)) == false)
	{
		vkDestroyBuffer(device, *outBuffer, nullptr);
		vkFreeMemory(device, *outMemory, nullptr);
		*outBuffer = VK_NULL_HANDLE;
		*outMemory = VK_NULL_HANDLE;
		return false;
	}

	return true;
}

bool VulkanRenderer::BeginImmediateCommands(VkCommandBuffer* outCommandBuffer)
{
	if (outCommandBuffer == nullptr)
	{
		return false;
	}

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	if (IsSuccess(vkAllocateCommandBuffers(device, &allocateInfo, outCommandBuffer)) == false)
	{
		return false;
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	return IsSuccess(vkBeginCommandBuffer(*outCommandBuffer, &beginInfo));
}

bool VulkanRenderer::EndImmediateCommands(VkCommandBuffer commandBuffer)
{
	if (IsSuccess(vkEndCommandBuffer(commandBuffer)) == false)
	{
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		return false;
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	const bool result =
		IsSuccess(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)) &&
		IsSuccess(vkQueueWaitIdle(graphicsQueue));

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

	return result;
}

bool VulkanRenderer::LoadShaderModule(const char* fileName, VkShaderModule* outShaderModule) const
{
	if (fileName == nullptr ||
		outShaderModule == nullptr)
	{
		return false;
	}

	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (file.is_open() == false)
	{
		return false;
	}

	const std::streamsize fileSize = file.tellg();

	if (fileSize <= 0 ||
		fileSize % 4 != 0)
	{
		return false;
	}

	std::vector<char> code(static_cast<size_t>(fileSize));
	file.seekg(0);
	file.read(code.data(), fileSize);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

	return IsSuccess(vkCreateShaderModule(device, &createInfo, nullptr, outShaderModule));
}

void VulkanRenderer::ReleaseSwapchainObjects()
{
	for (VkFramebuffer framebuffer : framebuffers)
	{
		if (framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	}

	framebuffers.clear();
	commandBuffers.clear();

	for (VkImageView imageView : swapchainImageViews)
	{
		if (imageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}
	}

	swapchainImageViews.clear();
	swapchainImages.clear();

	if (renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}

	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
}

void VulkanRenderer::ReleasePipelineObjects()
{
	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}

	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}
}

void VulkanRenderer::ReleaseTexture(VulkanTexture2D& texture)
{
	if (texture.imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device, texture.imageView, nullptr);
	}

	if (texture.image != VK_NULL_HANDLE)
	{
		vkDestroyImage(device, texture.image, nullptr);
	}

	if (texture.memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, texture.memory, nullptr);
	}

	texture = {};
}
