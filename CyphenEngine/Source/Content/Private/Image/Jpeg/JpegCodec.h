#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"

struct Resource;

// ============================================================================
// JpegCodec
// ----------------------------------------------------------------------------
// JPEG image leaf codec facade입니다.
//
// 책임:
//   - JPEG byte buffer를 Texture2D Resource로 변환
//   - 현재 build에서 선택된 JPEG provider에 실제 decode 위임
//
// 비책임:
//   - 확장자 dispatch
//   - File I/O
//   - ResourceId 발급
//   - Resource lifetime / cache
//   - GPU upload
// ============================================================================

class JpegCodec final
{
public:
	static bool Decode(const std::vector<uint8>& bytes, Resource& outResource);

private:
	JpegCodec() = delete;
	~JpegCodec() = delete;

	JpegCodec(const JpegCodec& other) = delete;
	JpegCodec& operator=(const JpegCodec& other) = delete;

	JpegCodec(JpegCodec&& other) = delete;
	JpegCodec& operator=(JpegCodec&& other) = delete;
};
