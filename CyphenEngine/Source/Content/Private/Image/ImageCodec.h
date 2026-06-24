#pragma once

#include <vector>

#include "Core/Public/CString.h"
#include "Core/Public/CPrimitiveTypes.h"

struct Resource;

// ============================================================================
// ImageCodec
// ----------------------------------------------------------------------------
// Image 계열 content codec dispatcher입니다.
//
// 책임:
//   - 이미지 확장자 table을 기준으로 image leaf codec 선택
//   - 이미지 파일 bytes를 ResourceKind::Texture2D 구체 자료형으로 decode
//
// 비책임:
//   - File I/O
//   - ResourceId 발급
//   - Resource lifetime / cache
//   - GPU upload
// ============================================================================

class ImageCodec final
{
public:
	static bool Decode(const CString& sourcePath, const std::vector<uint8>& bytes, Resource& outResource);

private:
	ImageCodec() = delete;
	~ImageCodec() = delete;

	ImageCodec(const ImageCodec& other) = delete;
	ImageCodec& operator=(const ImageCodec& other) = delete;

	ImageCodec(ImageCodec&& other) = delete;
	ImageCodec& operator=(ImageCodec&& other) = delete;
};
