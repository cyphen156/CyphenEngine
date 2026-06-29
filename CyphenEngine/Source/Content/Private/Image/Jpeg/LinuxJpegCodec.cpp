#include "pch.h"

#include <limits>

#include <turbojpeg.h>

#include "Content/Private/Image/Jpeg/LinuxJpegCodec.h"
#include "Resource/Public/Texture.h"

namespace
{
	// WindowsJpegCodec의 GUID_WICPixelFormat32bppRGBA 출력과 맞추기 위해
	// Linux 구현도 RGBA8 CPU-side texture buffer로 디코드합니다.
	constexpr int OUTPUT_PIXEL_FORMAT = TJPF_RGBA;
	constexpr uint32 OUTPUT_BYTES_PER_PIXEL = 4;

	bool IsValidTexture2D(const Texture2D& texture)
	{
		return texture.kind == ResourceKind::Texture2D &&
			texture.format == TextureFormat::Rgba8 &&
			texture.width > 0 &&
			texture.height > 0 &&
			static_cast<uint64>(texture.pixels.size()) ==
			static_cast<uint64>(texture.width) *
			static_cast<uint64>(texture.height) *
			OUTPUT_BYTES_PER_PIXEL;
	}

	bool IsValidJpegByteCount(std::size_t byteCount)
	{
		// libjpeg-turbo C API는 JPEG buffer size를 unsigned long으로 받습니다.
		// 엔진 byte buffer가 그 범위를 넘으면 안전하게 실패 처리합니다.
		return byteCount > 0 &&
			byteCount <=
			static_cast<std::size_t>(std::numeric_limits<unsigned long>::max());
	}

	bool IsValidDecodedSize(int width, int height)
	{
		// JPEG header에서 얻은 크기는 int입니다.
		// Texture2D는 uint32 크기 정책을 사용하므로 여기서 범위를 검증합니다.
		if (width <= 0 || height <= 0)
		{
			return false;
		}

		if (static_cast<uint64>(width) >
			static_cast<uint64>(std::numeric_limits<uint32>::max()))
		{
			return false;
		}

		if (static_cast<uint64>(height) >
			static_cast<uint64>(std::numeric_limits<uint32>::max()))
		{
			return false;
		}

		return true;
	}

	bool TryGetDecodedByteCount(int width, int height, uint64& outByteCount)
	{
		if (!IsValidDecodedSize(width, height))
		{
			return false;
		}

		const uint64 pixelCount =
			static_cast<uint64>(width) *
			static_cast<uint64>(height);

		if (pixelCount >
			std::numeric_limits<uint64>::max() / OUTPUT_BYTES_PER_PIXEL)
		{
			return false;
		}

		outByteCount = pixelCount * OUTPUT_BYTES_PER_PIXEL;

		if (outByteCount >
			static_cast<uint64>(std::numeric_limits<std::size_t>::max()))
		{
			return false;
		}

		return true;
	}

	bool TryGetStride(int width, int& outStride)
	{
		// TurboJPEG pitch는 한 줄의 byte 수입니다.
		// RGBA8이므로 width * 4입니다.
		if (width <= 0)
		{
			return false;
		}

		if (static_cast<uint64>(width) >
			static_cast<uint64>(std::numeric_limits<int>::max()) /
			OUTPUT_BYTES_PER_PIXEL)
		{
			return false;
		}

		outStride = width * static_cast<int>(OUTPUT_BYTES_PER_PIXEL);

		return true;
	}
}

bool LinuxJpegCodec::Decode(const std::vector<uint8>& bytes, Texture2D& outTexture)
{
	// 실패하더라도 기존 ResourceId는 보존합니다.
	// WindowsJpegCodec과 같은 복구 정책입니다.
	const ResourceId resourceId = outTexture.resourceId;

	outTexture = {};
	outTexture.resourceId = resourceId;
	outTexture.kind = ResourceKind::Texture2D;

	if (!IsValidJpegByteCount(bytes.size()))
	{
		return false;
	}

	tjhandle handle = tjInitDecompress();

	if (handle == nullptr)
	{
		return false;
	}

	bool IsDecoded = false;

	{
		const unsigned char* jpegBytes =
			reinterpret_cast<const unsigned char*>(bytes.data());

		const unsigned long jpegByteCount =
			static_cast<unsigned long>(bytes.size());

		int width = 0;
		int height = 0;
		int jpegSubsampling = 0;
		int jpegColorSpace = 0;

		// 먼저 JPEG header만 읽어서 출력 Texture 크기를 결정합니다.
		int result = tjDecompressHeader3(
			handle,
			jpegBytes,
			jpegByteCount,
			&width,
			&height,
			&jpegSubsampling,
			&jpegColorSpace);

		uint64 byteCount = 0;

		if (result == 0)
		{
			if (!TryGetDecodedByteCount(width, height, byteCount))
			{
				result = -1;
			}
		}

		int stride = 0;

		if (result == 0)
		{
			if (!TryGetStride(width, stride))
			{
				result = -1;
			}
		}

		if (result == 0)
		{
			outTexture.format = TextureFormat::Rgba8;
			outTexture.width = static_cast<uint32>(width);
			outTexture.height = static_cast<uint32>(height);
			outTexture.pixels.resize(static_cast<std::size_t>(byteCount));

			// TurboJPEG가 JPEG byte buffer를 RGBA8 packed pixel buffer로 디코드합니다.
			result = tjDecompress2(
				handle,
				jpegBytes,
				jpegByteCount,
				reinterpret_cast<unsigned char*>(outTexture.pixels.data()),
				width,
				stride,
				height,
				OUTPUT_PIXEL_FORMAT,
				TJFLAG_ACCURATEDCT);

			if (result == 0)
			{
				IsDecoded = IsValidTexture2D(outTexture);
			}
		}
	}

	tjDestroy(handle);

	if (IsDecoded == false)
	{
		outTexture = {};
		outTexture.resourceId = resourceId;
		outTexture.kind = ResourceKind::Texture2D;
	}

	return IsDecoded;
}
