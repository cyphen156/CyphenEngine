#include "pch.h"

#include <limits>
#include <wincodec.h>
#include <wrl/client.h>

#include "Content/Private/Image/Jpeg/WindowsJpegCodec.h"
#include "Resource/Public/Texture.h"

using Microsoft::WRL::ComPtr;

namespace
{
	bool IsValidTexture2D(const Texture2D& texture)
	{
		return texture.kind == ResourceKind::Texture2D &&
			texture.format == TextureFormat::Rgba8 &&
			texture.width > 0 &&
			texture.height > 0 &&
			static_cast<uint64>(texture.pixels.size()) ==
			static_cast<uint64>(texture.width) *
			static_cast<uint64>(texture.height) *
			4;
	}
}

bool WindowsJpegCodec::Decode(const std::vector<uint8>& bytes, Texture2D& outTexture)
{
	const ResourceId resourceId = outTexture.resourceId;

	outTexture = {};
	outTexture.resourceId = resourceId;
	outTexture.kind = ResourceKind::Texture2D;

	if (bytes.empty() ||
		bytes.size() > static_cast<size_t>(std::numeric_limits<DWORD>::max()))
	{
		return false;
	}

	const HRESULT initResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(initResult) && initResult != RPC_E_CHANGED_MODE)
	{
		return false;
	}

	const bool IsInitialized = SUCCEEDED(initResult);
	bool IsDecoded = false;

	{
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICBitmapFrameDecode> frame;
		ComPtr<IWICFormatConverter> converter;

		HRESULT result = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(factory.GetAddressOf()));

		if (SUCCEEDED(result))
		{
			result = factory->CreateStream(stream.GetAddressOf());
		}

		if (SUCCEEDED(result))
		{
			result = stream->InitializeFromMemory(
				const_cast<BYTE*>(bytes.data()),
				static_cast<DWORD>(bytes.size()));
		}

		if (SUCCEEDED(result))
		{
			result = factory->CreateDecoderFromStream(
				stream.Get(),
				nullptr,
				WICDecodeMetadataCacheOnDemand,
				decoder.GetAddressOf());
		}

		if (SUCCEEDED(result))
		{
			result = decoder->GetFrame(0, frame.GetAddressOf());
		}

		if (SUCCEEDED(result))
		{
			result = factory->CreateFormatConverter(converter.GetAddressOf());
		}

		if (SUCCEEDED(result))
		{
			result = converter->Initialize(
				frame.Get(),
				GUID_WICPixelFormat32bppRGBA,
				WICBitmapDitherTypeNone,
				nullptr,
				0.0,
				WICBitmapPaletteTypeCustom);
		}

		UINT width = 0;
		UINT height = 0;

		if (SUCCEEDED(result))
		{
			result = converter->GetSize(&width, &height);
		}

		if (SUCCEEDED(result) && width > 0 && height > 0)
		{
			const uint64 byteCount =
				static_cast<uint64>(width) *
				static_cast<uint64>(height) *
				4;

			if (byteCount <= static_cast<uint64>(std::numeric_limits<UINT>::max()))
			{
				const UINT stride = width * 4;

				outTexture.format = TextureFormat::Rgba8;
				outTexture.width = width;
				outTexture.height = height;
				outTexture.pixels.resize(static_cast<size_t>(byteCount));

				result = converter->CopyPixels(
					nullptr,
					stride,
					static_cast<UINT>(byteCount),
					outTexture.pixels.data());

				if (SUCCEEDED(result))
				{
					IsDecoded = IsValidTexture2D(outTexture);
				}
			}
		}
	}

	if (IsInitialized)
	{
		CoUninitialize();
	}

	if (IsDecoded == false)
	{
		outTexture = {};
		outTexture.resourceId = resourceId;
		outTexture.kind = ResourceKind::Texture2D;
	}

	return IsDecoded;
}
