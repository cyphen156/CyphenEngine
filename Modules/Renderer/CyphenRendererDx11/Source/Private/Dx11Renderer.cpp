#include <cstring>
#include <d3dcompiler.h>
#include <limits>

#include "Dx11Renderer.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace
{
	struct TexturedQuadVertex
	{
		float position[3] = {};
		float uv[2] = {};
	};

	static_assert(sizeof(TexturedQuadVertex) == 20, "TexturedQuadVertex must be 20 bytes.");

	const char TexturedQuadShaderSource[] =
		"struct VSInput"
		"{"
		"	float3 position : POSITION;"
		"	float2 uv : TEXCOORD0;"
		"};"
		"struct VSOutput"
		"{"
		"	float4 position : SV_POSITION;"
		"	float2 uv : TEXCOORD0;"
		"};"
		"VSOutput VsMain(VSInput input)"
		"{"
		"	VSOutput output;"
		"	output.position = float4(input.position, 1.0f);"
		"	output.uv = input.uv;"
		"	return output;"
		"}"
		"Texture2D Texture0 : register(t0);"
		"SamplerState Sampler0 : register(s0);"
		"float4 PsMain(VSOutput input) : SV_TARGET"
		"{"
		"	return Texture0.Sample(Sampler0, input.uv);"
		"}";
}

bool Dx11Renderer::Initialize(void* nativeRenderContextHandle, const NativeWindowInfo& windowInfo)
{
	(void)nativeRenderContextHandle;

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
	
	renderTargetWidth = windowInfo.windowWidth;
	renderTargetHeight = windowInfo.windowHeight;

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

		case RenderCommandType::DrawTexturedQuad:
			if (ExecuteDrawTexturedQuad(payloadWords, header.payloadWordCount) == false)
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
bool Dx11Renderer::ExecuteResourceCommandList(const ResourceCommandList& commandList)
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

bool Dx11Renderer::ExecuteDrawTexturedQuad(const RenderCommandWord* payloadWords, uint32 payloadWordCount)
{
	if (payloadWords == nullptr ||
		payloadWordCount != 1 ||
		deviceContext == nullptr ||
		renderTargetView == nullptr ||
		renderTargetWidth == 0 ||
		renderTargetHeight == 0)
	{
		return false;
	}

	DrawTexturedQuadCommand command = {};
	std::memcpy(&command, payloadWords, sizeof(command));

	Dx11Texture2D* texture = FindTexture2D(command.textureId);

	if (texture == nullptr)
	{
		return false;
	}

	if (EnsureTexturedQuadPipeline() == false)
	{
		return false;
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(renderTargetWidth);
	viewport.Height = static_cast<float>(renderTargetHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	const UINT stride = sizeof(TexturedQuadVertex);
	const UINT offset = 0;

	ID3D11Buffer* vertexBuffer = texturedQuadVertexBuffer.Get();
	deviceContext->IASetInputLayout(texturedQuadInputLayout.Get());
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(texturedQuadIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->VSSetShader(texturedQuadVertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(texturedQuadPixelShader.Get(), nullptr, 0);

	ID3D11ShaderResourceView* shaderResourceView = texture->shaderResourceView.Get();
	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);

	ID3D11SamplerState* sampler = texturedQuadSampler.Get();
	deviceContext->PSSetSamplers(0, 1, &sampler);

	deviceContext->DrawIndexed(6, 0, 0);

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

#ifdef _DEBUG
bool Dx11Renderer::ExecuteUploadResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount)
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

bool Dx11Renderer::UploadTexture2D(
	const UploadResourceCommand& command,
	const uint8* payloadBytes,
	uint64 payloadByteCount)
{
	if (device == nullptr ||
		payloadBytes == nullptr ||
		payloadByteCount < sizeof(Texture2DUploadPayload))
	{
		return false;
	}

	Texture2DUploadPayload texturePayload = {};
	std::memcpy(&texturePayload, payloadBytes, sizeof(texturePayload));

	if (texturePayload.format != TextureFormat::Rgba8 ||
		texturePayload.width == 0 ||
		texturePayload.height == 0)
	{
		return false;
	}

	const uint64 pixelByteCount =
		payloadByteCount -
		static_cast<uint64>(sizeof(texturePayload));

	const uint64 expectedPixelByteCount =
		static_cast<uint64>(texturePayload.width) *
		static_cast<uint64>(texturePayload.height) *
		4;

	if (pixelByteCount != expectedPixelByteCount ||
		expectedPixelByteCount > static_cast<uint64>((std::numeric_limits<uint32>::max)()))
	{
		return false;
	}

	const uint8* pixels = payloadBytes + sizeof(texturePayload);

	D3D11_TEXTURE2D_DESC textureDescription = {};
	textureDescription.Width = texturePayload.width;
	textureDescription.Height = texturePayload.height;
	textureDescription.MipLevels = 1;
	textureDescription.ArraySize = 1;
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.Usage = D3D11_USAGE_IMMUTABLE;
	textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = pixels;
	initialData.SysMemPitch = texturePayload.width * 4;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	HRESULT result = device->CreateTexture2D(
		&textureDescription,
		&initialData,
		texture.GetAddressOf());

	if (FAILED(result) ||
		texture == nullptr)
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription = {};
	shaderResourceViewDescription.Format = textureDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescription.Texture2D.MipLevels = 1;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	result = device->CreateShaderResourceView(
		texture.Get(),
		&shaderResourceViewDescription,
		shaderResourceView.GetAddressOf());

	if (FAILED(result) ||
		shaderResourceView == nullptr)
	{
		return false;
	}

	if (command.resourceId > static_cast<ResourceId>((std::numeric_limits<uint32>::max)()))
	{
		return false;
	}

	const size_t tableIndex = static_cast<size_t>(command.resourceId);

	if (texture2DTable.size() <= tableIndex)
	{
		texture2DTable.resize(tableIndex + 1);
	}

	Dx11Texture2D& textureEntry = texture2DTable[tableIndex];
	textureEntry.resourceId = command.resourceId;
	textureEntry.texture = texture;
	textureEntry.shaderResourceView = shaderResourceView;
	textureEntry.width = texturePayload.width;
	textureEntry.height = texturePayload.height;

	return true;
}

bool Dx11Renderer::ExecuteDestroyResource(const ResourceCommandWord* payloadWords, uint32 payloadWordCount)
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

	if (tableIndex >= texture2DTable.size())
	{
		return true;
	}

	texture2DTable[tableIndex] = {};

	return true;
}
#endif

bool Dx11Renderer::EnsureTexturedQuadPipeline()
{
	if (IsTexturedQuadPipelineReady)
	{
		return true;
	}

	if (device == nullptr)
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HRESULT result = D3DCompile(
		TexturedQuadShaderSource,
		sizeof(TexturedQuadShaderSource),
		nullptr,
		nullptr,
		nullptr,
		"VsMain",
		"vs_4_0",
		0,
		0,
		vertexShaderBlob.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	result = D3DCompile(
		TexturedQuadShaderSource,
		sizeof(TexturedQuadShaderSource),
		nullptr,
		nullptr,
		nullptr,
		"PsMain",
		"ps_4_0",
		0,
		0,
		pixelShaderBlob.GetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	result = device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		nullptr,
		texturedQuadVertexShader.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	result = device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr,
		texturedQuadPixelShader.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	result = device->CreateInputLayout(
		inputElements,
		static_cast<UINT>(sizeof(inputElements) / sizeof(inputElements[0])),
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		texturedQuadInputLayout.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	const TexturedQuadVertex vertices[] =
	{
		{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }
	};

	D3D11_BUFFER_DESC vertexBufferDescription = {};
	vertexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(vertices));
	vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertices;

	result = device->CreateBuffer(
		&vertexBufferDescription,
		&vertexBufferData,
		texturedQuadVertexBuffer.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	const uint16 indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	D3D11_BUFFER_DESC indexBufferDescription = {};
	indexBufferDescription.ByteWidth = static_cast<UINT>(sizeof(indices));
	indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices;

	result = device->CreateBuffer(
		&indexBufferDescription,
		&indexBufferData,
		texturedQuadIndexBuffer.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDescription.MinLOD = 0.0f;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(
		&samplerDescription,
		texturedQuadSampler.GetAddressOf());

	if (FAILED(result))
	{
		ReleaseTexturedQuadPipeline();
		return false;
	}

	IsTexturedQuadPipelineReady = true;

	return true;
}

void Dx11Renderer::ReleaseTexturedQuadPipeline()
{
	texturedQuadSampler.Reset();
	texturedQuadIndexBuffer.Reset();
	texturedQuadVertexBuffer.Reset();
	texturedQuadInputLayout.Reset();
	texturedQuadPixelShader.Reset();
	texturedQuadVertexShader.Reset();
	IsTexturedQuadPipelineReady = false;
}

Dx11Texture2D* Dx11Renderer::FindTexture2D(ResourceId resourceId)
{
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

	Dx11Texture2D& texture = texture2DTable[tableIndex];

	if (texture.resourceId != resourceId ||
		texture.shaderResourceView == nullptr)
	{
		return nullptr;
	}

	return &texture;
}
