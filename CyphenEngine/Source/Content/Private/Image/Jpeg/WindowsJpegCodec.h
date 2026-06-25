#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"

struct Texture2D;

// ============================================================================
// WindowsJpegCodec
// ----------------------------------------------------------------------------
// Windows WIC 기반 JPEG provider입니다.
//
// 이 클래스는 HAL / Platform abstraction이 아니라 Content JPEG codec의
// Windows provider입니다. WIC / COM 세부 구현을 JPEG provider 내부에 가둡니다.
//
// 장기적으로 libjpeg-turbo 같은 provider를 추가하더라도 JpegCodec의 public
// facade는 유지됩니다.
// ============================================================================

class WindowsJpegCodec final
{
public:
	static bool Decode(const std::vector<uint8>& bytes, Texture2D& outTexture);

private:
	WindowsJpegCodec() = delete;
	~WindowsJpegCodec() = delete;

	WindowsJpegCodec(const WindowsJpegCodec& other) = delete;
	WindowsJpegCodec& operator=(const WindowsJpegCodec& other) = delete;

	WindowsJpegCodec(WindowsJpegCodec&& other) = delete;
	WindowsJpegCodec& operator=(WindowsJpegCodec&& other) = delete;
};
