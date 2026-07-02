#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"

struct Texture2D;

// ============================================================================
// LinuxJpegCodec
// ----------------------------------------------------------------------------
// Linux 전용 JPEG 디코더 구현입니다.
//
// libjpeg-turbo의 TurboJPEG API를 사용해 JPEG byte buffer를
// CPU-side Texture2D RGBA8 pixel buffer로 디코드합니다.
//
// 비책임:
//     파일 읽기
//     Resource 종류 판별
//     GPU texture 생성
//     Renderer backend resource 등록
//
// 규칙:
//     Decode 실패 시 false를 반환합니다.
//     outTexture의 ResourceId는 보존합니다.
// ============================================================================

class LinuxJpegCodec final
{
public:
	static bool Decode(
		const std::vector<uint8>& bytes,
		Texture2D& outTexture);

private:
	LinuxJpegCodec() = delete;
	~LinuxJpegCodec() = delete;

	LinuxJpegCodec(const LinuxJpegCodec& other) = delete;
	LinuxJpegCodec& operator=(const LinuxJpegCodec& other) = delete;

	LinuxJpegCodec(LinuxJpegCodec&& other) = delete;
	LinuxJpegCodec& operator=(LinuxJpegCodec&& other) = delete;
};
