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
// Linux provider는 libjpeg-turbo를 사용하며, 두 플랫폼 구현 모두
// JpegCodec facade 뒤에서 같은 Texture2D RGBA8 결과 계약을 제공합니다.
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
