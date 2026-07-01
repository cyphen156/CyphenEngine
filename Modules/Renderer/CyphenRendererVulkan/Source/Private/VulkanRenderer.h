#pragma once

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RenderCommand.h"

#ifdef _DEBUG
#include "Modules/Resource/Public/ResourceCommand.h"
#endif

// ============================================================================
// VulkanRenderer
// ----------------------------------------------------------------------------
// Renderer Backend ABI 계약을 Vulkan으로 구현하는 구현체입니다.
//
// 책임:
//   - Vulkan Instance / Surface / Device / Swapchain 수명 관리
//   - RenderCommandList를 Vulkan command buffer 제출로 변환
//   - Debug ResourceCommandList의 Texture2D 업로드를 VkImage로 변환
//   - 현재 first-light 범위에서 loaded texture를 full-screen quad로 출력
//
// 비책임:
//   - 파일 I/O / 이미지 코덱 decode
//   - Engine Frame 생산
//   - 플랫폼별 native surface 생성 세부 구현
//   - resize 대응 / swapchain recreate / advanced frame scheduling
//
// #3_3 Windows first-light 단계:
//   - SDK 헤더/링크 기반 Vulkan 초기화
//   - Swapchain / RenderPass / Pipeline 생성
//   - Debug Texture2D UploadResource를 실제 VkImage로 업로드
//   - DrawTexturedQuad command의 textureId를 렌더링
// ============================================================================

// ============================================================================
// VulkanTexture2D
// ----------------------------------------------------------------------------
// Renderer ResourceCommand로 전달된 Texture2D 하나를 Vulkan GPU 리소스로
// 보관하는 backend 내부 테이블 엔트리입니다.
//
// 책임:
//   - Engine ResourceId와 VkImage 계열 객체의 대응 관계 보관
//   - DrawTexturedQuad에서 바로 bind 가능한 descriptor set 보관
//
// 비책임:
//   - CPU-side decoded pixel 소유
//   - ResourceId 발급
//   - descriptor pool 전체 수명 관리
// ============================================================================

struct VulkanTexture2D
{
	ResourceId resourceId = InvalidResourceId;
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	uint32 width = 0;
	uint32 height = 0;
};

// ============================================================================
// VulkanQueueFamilySelection
// ----------------------------------------------------------------------------
// Vulkan physical device 선택 과정에서 찾은 graphics/present queue family
// index 쌍입니다.
//
// 책임:
//   - logical device 생성과 swapchain image sharing mode 결정에 필요한
//     queue family 선택 결과를 보관합니다.
// ============================================================================

struct VulkanQueueFamilySelection
{
	uint32 graphicsFamily = UINT32_MAX;
	uint32 presentFamily = UINT32_MAX;

	bool IsComplete() const;
};

class VulkanRenderer final
{
public:
	VulkanRenderer() = default;
	~VulkanRenderer() = default;

	VulkanRenderer(const VulkanRenderer& other) = delete;
	VulkanRenderer& operator=(const VulkanRenderer& other) = delete;

	VulkanRenderer(VulkanRenderer&& other) = delete;
	VulkanRenderer& operator=(VulkanRenderer&& other) = delete;

	bool Initialize(const NativeWindowInfo& windowInfo);
	void Shutdown();

	bool ExecuteCommandList(const RenderCommandList& commandList);

#ifdef _DEBUG
	bool ExecuteResourceCommandList(const ResourceCommandList& commandList);
#endif

private:
	bool CreateInstance();
	bool CreateSurface(const NativeWindowInfo& windowInfo);
	bool SelectPhysicalDevice();
	bool CreateDevice();
	bool CreateSwapchain(const NativeWindowInfo& windowInfo);
	bool CreateRenderPass();
	bool CreateFramebuffers();
	bool CreateCommandPool();
	bool CreateDescriptorObjects();
	bool CreatePipeline();
	bool CreateSyncObjects();

	bool ExecuteClearRenderTarget(const RenderCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecuteDrawTexturedQuad(const RenderCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecutePresent(uint32 payloadWordCount);
	bool RenderAndPresent();

#ifdef _DEBUG
	bool ExecuteUploadResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecuteDestroyResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount);
	bool UploadTexture2D(const UploadResourceCommand& command, const uint8* payloadBytes, uint64 payloadByteCount);
#endif

	VulkanTexture2D* FindTexture2D(ResourceId resourceId);
	uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const;
	bool CreateBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer* outBuffer,
		VkDeviceMemory* outMemory);
	bool BeginImmediateCommands(VkCommandBuffer* outCommandBuffer);
	bool EndImmediateCommands(VkCommandBuffer commandBuffer);
	bool LoadShaderModule(const char* fileName, VkShaderModule* outShaderModule) const;
	void ReleaseSwapchainObjects();
	void ReleasePipelineObjects();
	void ReleaseTexture(VulkanTexture2D& texture);

private:
	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VulkanQueueFamilySelection queueFamilies;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D swapchainExtent = {};
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> framebuffers;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;

	VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
	VkFence inFlightFence = VK_NULL_HANDLE;

	float pendingClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ResourceId pendingTextureId = InvalidResourceId;

#ifdef _DEBUG
	std::vector<VulkanTexture2D> texture2DTable;
#endif
};
