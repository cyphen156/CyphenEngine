#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <vector>

#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RenderCommand.h"

#ifdef _DEBUG
#include "Modules/Resource/Public/ResourceCommand.h"
#endif

struct Dx11Texture2D
{
	ResourceId resourceId = InvalidResourceId;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	uint32 width = 0;
	uint32 height = 0;
};

// ============================================================================
// Dx11Renderer
// ----------------------------------------------------------------------------
// Renderer Backend ABI 계약을 Direct3D 11로 구현하는 구현체입니다.
//
// 책임:
//   - D3D11 Device / ImmediateContext / SwapChain 생성
//   - BackBuffer RenderTargetView 생성
//   - RenderCommand IR 해석
//   - Clear / TexturedQuad draw / Present 실행
//   - Debug ResourceCommand의 Texture2D upload / destroy 처리
//
// 현재 범위:
//   - ResourceId 기반 Texture2D table과 textured quad pipeline을 제공합니다.
//   - 정식 Mesh / Material / ResourceManager와 일반화된 그래픽스 API는 후속
//     Renderer 구현 확장 범위입니다.
// ============================================================================

class Dx11Renderer final
{
public:
	Dx11Renderer() = default;
	~Dx11Renderer() = default;

	Dx11Renderer(const Dx11Renderer& other) = delete;
	Dx11Renderer& operator=(const Dx11Renderer& other) = delete;

	Dx11Renderer(Dx11Renderer&& other) = delete;
	Dx11Renderer& operator=(Dx11Renderer&& other) = delete;

	bool Initialize(void* nativeRenderContextHandle, const NativeWindowInfo& windowInfo);
	void Shutdown();

	bool ExecuteCommandList(const RenderCommandList& commandList);

#ifdef _DEBUG
	bool ExecuteResourceCommandList(const ResourceCommandList& commandList);
#endif

private:
	bool CreateRenderTargetView();

	bool ExecuteClearRenderTarget(const RenderCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecuteDrawTexturedQuad(const RenderCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecutePresent(uint32 payloadWordCount);

#ifdef _DEBUG
	bool ExecuteUploadResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecuteDestroyResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount);
	bool UploadTexture2D(const UploadResourceCommand& command, const uint8* payloadBytes, uint64 payloadByteCount);
#endif

	bool EnsureTexturedQuadPipeline();
	void ReleaseTexturedQuadPipeline();

	Dx11Texture2D* FindTexture2D(ResourceId resourceId);

private:
	// D3D11 GPU resource와 pipeline object를 생성하는 device입니다.
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	// Resource binding, draw와 render-target 명령을 실행하는 immediate context입니다.
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	// Native Window에 연결된 현재 double-buffer swap chain입니다.
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	// SwapChain BackBuffer에 연결된 출력 Render Target View입니다.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	uint32 renderTargetWidth = 0;
	uint32 renderTargetHeight = 0;

	std::vector<Dx11Texture2D> texture2DTable;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> texturedQuadVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> texturedQuadPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> texturedQuadInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> texturedQuadVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> texturedQuadIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> texturedQuadSampler;
	bool IsTexturedQuadPipelineReady = false;
};
