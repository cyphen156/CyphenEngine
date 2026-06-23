#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include "HAL/Public/NativeWindowInfo.h"
#include "Modules/Renderer/Public/RenderCommand.h"

// ============================================================================
// Dx11Renderer
// ----------------------------------------------------------------------------
// Renderer Backend ABI 계약을 Direct3D 11로 구현하는 구현체입니다.
//
// 책임:
//   - D3D11 Device / ImmediateContext / SwapChain 생성
//   - BackBuffer RenderTargetView 생성
//   - RenderCommand IR 해석
//   - Clear / Present 실행
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

	bool Initialize(const NativeWindowInfo& windowInfo);
	void Shutdown();

	bool ExecuteCommandList(const RenderCommandList& commandList);

private:
	bool CreateRenderTargetView();

	bool ExecuteClearRenderTarget(const RenderCommandWord* payloadWords, uint32 payloadWordCount);
	bool ExecutePresent(uint32 payloadWordCount);

private:
	// GPU 리소스 Dx11 디바이스(Buffer / Texture / Shader / RenderTargetView / Pipeline State)
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	// GPU 명령 실행라인 Context(Bind : Shader / Buffer, Texture / Draw, Dispatch / RenderTargeting / Clear)
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	// 실제 네이티브에 연결되는 Chain - 현재 더블 버퍼 사용중
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	// SwapChain BackBuffer에 연결된 출력 Render Target View입니다.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};