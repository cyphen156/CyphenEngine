#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/FileTypes.h"

// ============================================================================
// TextCodec
// ----------------------------------------------------------------------------
// CString과 raw byte buffer 사이의 텍스트 변환 계층.
//
// TextCodec은 CChar 정책과 TextEncoding 정책을 연결합니다.
//
// 책임:
//     CString을 전달받은 TextEncoding 기준의 byte 배열로 변환합니다.
//     byte 배열을 전달받은 TextEncoding 기준으로 CString으로 복원합니다.
//     LineEnding 정책을 적용합니다.
//
// 반환 정책:
//     성공 / 실패만 반환합니다.
//     세부 실패 원인 기록은 이후 Diagnostics 계층에서 처리합니다.
//
// 규칙:
//     호출자가 전달한 TextEncoding만 신뢰합니다.
//     TextCodec은 인코딩을 추측하지 않습니다.
// ============================================================================

class TextCodec final
{
public:
	static TextEncoding GetDefaultTextEncoding();
	static TextEncoding ResolveEncoding(const std::vector<uint8>& bytes, TextEncoding fallbackEncoding);

	static bool Encode(
		const CString& text,
		std::vector<uint8>& outBytes,
		TextEncoding encoding,
		LineEnding lineEnding);

	static bool Decode(
		const std::vector<uint8>& bytes,
		CString& outText,
		TextEncoding encoding);

private:
	TextCodec() = delete;
	~TextCodec() = delete;

	TextCodec(const TextCodec& other) = delete;
	TextCodec& operator=(const TextCodec& other) = delete;

	TextCodec(TextCodec&& other) = delete;
	TextCodec& operator=(TextCodec&& other) = delete;
};
