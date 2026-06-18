#pragma once

// ============================================================================
// FileTypes
// ----------------------------------------------------------------------------
// CyphenEngine File 계층에서 사용하는 텍스트 정책 타입.
//
// File 공개 API는 UE식 모델을 따라 세부 실패 사유를 반환하지 않습니다.
// File은 성공 / 실패 여부만 반환합니다.
// 실패 원인 기록은 이후 Diagnostics / Logger / Platform diagnostics 계층에서
// 처리합니다.
//
// TextEncoding은 텍스트 파일의 외부 저장 인코딩 정책입니다.
// 이는 CChar 정책과 다릅니다.
//
// CChar 정책:
//     CString 내부 코드 유닛을 정의합니다.
//
// TextEncoding:
//     텍스트가 파일에 어떤 인코딩으로 저장되는지를 정의합니다.
//
// CString <-> byte 변환은 TextCodec 책임입니다.
// File은 인코딩을 자동 판별하지 않습니다.
// File은 호출자가 전달한 TextEncoding 기준으로만 동작합니다.
//
// Ansi는 레거시 호환용 옵션입니다.
// 줄바꿈 정책은 인코딩과 결합하지 않습니다.
// ============================================================================

enum class TextEncoding
{
	Utf8,
	Utf8WithBom,

	Utf16LE,
	Utf16BE,

	Ansi
};

enum class LineEnding
{
	LF,
	CRLF,
	Preserve   // 줄바꿈을 변환하지 않고 원본 그대로 둔다(Append 등 verbatim 경로).
};
