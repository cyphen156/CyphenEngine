#include "../Public/Dx11Renderer.h"

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

	return device != nullptr &&
		deviceContext != nullptr &&
		swapChain != nullptr;
}

void Dx11Renderer::Shutdown()
{
	if (deviceContext != nullptr)
	{
		deviceContext->ClearState();
		deviceContext->Flush();
	}

	swapChain.Reset();
	deviceContext.Reset();
	device.Reset();
}