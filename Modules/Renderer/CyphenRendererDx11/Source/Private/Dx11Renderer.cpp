#include "Dx11Renderer.h"

#include <cstring>

bool Dx11Renderer::Initialize(const NativeWindowInfo& windowInfo)
{
	if (windowInfo.nativeWindowHandle == nullptr ||
		windowInfo.windowWidth == 0 ||
		windowInfo.windowHeight == 0)
	{
		return false;
	}

	HWND windowHandle = static_cast<HWND>(windowInfo.nativeWindowHandle);

	if (::IsWindow(windowHandle) == FALSE)
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
	swapChainDescription.BufferDesc.Width = windowInfo.windowWidth;
	swapChainDescription.BufferDesc.Height = windowInfo.windowHeight;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 2;
	swapChainDescription.OutputWindow = windowHandle;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D_FEATURE_LEVEL selectedFeatureLevel = D3D_FEATURE_LEVEL_10_0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		static_cast<UINT>(sizeof(featureLevels) / sizeof(featureLevels[0])),
		D3D11_SDK_VERSION,
		&swapChainDescription,
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		&selectedFeatureLevel,
		deviceContext.GetAddressOf());

#ifdef _DEBUG
	if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
	{
		Shutdown();

		createDeviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;

		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevels,
			static_cast<UINT>(sizeof(featureLevels) / sizeof(featureLevels[0])),
			D3D11_SDK_VERSION,
			&swapChainDescription,
			swapChain.GetAddressOf(),
			device.GetAddressOf(),
			&selectedFeatureLevel,
			deviceContext.GetAddressOf());
	}
#endif

	if (FAILED(result))
	{
		Shutdown();
		return false;
	}

	(void)selectedFeatureLevel;

	if (device == nullptr ||
		deviceContext == nullptr ||
		swapChain == nullptr)
	{
		Shutdown();
		return false;
	}

	if (CreateRenderTargetView() == false)
	{
		Shutdown();
		return false;
	}

	return true;
}

void Dx11Renderer::Shutdown()
{
	if (deviceContext != nullptr)
	{
		deviceContext->ClearState();
		deviceContext->Flush();
	}

	renderTargetView.Reset();
	swapChain.Reset();
	deviceContext.Reset();
	device.Reset();
}

bool Dx11Renderer::ExecuteCommandList(const RenderCommandList& commandList)
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
		RenderCommandHeader header = {};
		std::memcpy(&header, &commandList.words[cursor], sizeof(header));

		++cursor;

		// cursor + payloadWordCount는 uint32 overflow 가능성이 있으므로,
		// 남은 word 수에서 payloadWordCount를 비교합니다.
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

bool Dx11Renderer::CreateRenderTargetView()
{
	if (device == nullptr ||
		swapChain == nullptr)
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	HRESULT result = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf()));

	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		renderTargetView.GetAddressOf());

	return SUCCEEDED(result) && renderTargetView != nullptr;
}

bool Dx11Renderer::ExecuteClearRenderTarget(
	const RenderCommandWord* payloadWords,
	uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount != 2 ||
		deviceContext == nullptr ||
		renderTargetView == nullptr)
	{
		return false;
	}

	ClearRenderTargetCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), command.color);

	return true;
}

bool Dx11Renderer::ExecutePresent(uint32 payloadWordCount)
{
	if (payloadWordCount != 0 ||
		swapChain == nullptr)
	{
		return false;
	}

	HRESULT result = swapChain->Present(0, 0);

	return SUCCEEDED(result);
}