#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include "HAL/Public/NativeWindowInfo.h"

// ============================================================================
// Dx11Renderer
// ----------------------------------------------------------------------------
// Renderer 실행 계약을 Direct3D 11로 실현하는 구현체입니다.
//
// #2_4 책임:
//   - D3D11 Device 생성·파괴
//   - ImmediateContext 생성·파괴
//   - DXGI SwapChain 생성·파괴
//
// 생성과 파괴는 Renderer가 소유한 Render Thread에서 수행합니다.
//
// 이월:
//   - RenderTargetView
//   - Clear / Present
//   - Command List 해석
//   - Capability 조회
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

private:
	// Buffer / Texture / Shader / View와 Pipeline 상태 객체를 생성하는 DX11 Device
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	// Shader / Buffer / Texture / RenderTarget을 바인딩하고
	// Draw / Dispatch / Clear 명령을 제출하는 DX11 Immediate Context
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	// 렌더링 결과를 Native Window에 표시하는 Flip Model SwapChain
	// 현재 2개의 Back Buffer를 사용합니다.
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
};