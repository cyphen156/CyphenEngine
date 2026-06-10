#pragma once

#include "Build/Public/define.h"

// ============================================================================
// CChar
// ----------------------------------------------------------------------------
// CyphenEngine 내부 문자열 코드 유닛 정책.
//
// CChar 정책은 Core가 결정하지 않습니다.
// Build / define.h 또는 빌드 시스템에서 주입된 정책을 소비합니다.
//
// CString은 엔진 내부 문자열 타입입니다.
// 외부 파일 저장 인코딩은 TextEncoding으로 별도 관리합니다.
//
// 주의:
//     CString::length()는 사람이 보는 글자 수가 아닙니다.
//     CString::length()는 CChar 코드 유닛 개수입니다.
// ============================================================================

using CAnsiChar = char;
using CUtf8Char = char;
using CUtf16Char = char16_t;
using CWideChar = wchar_t;
using CUtf32Char = char32_t;

#define CANSI_TEXT(str) str
#define CUTF8_TEXT(str) str
#define CUTF16_TEXT(str) u##str
#define CWIDE_TEXT(str) L##str
#define CUTF32_TEXT(str) U##str

#if CCHAR_IS_UTF8

using CChar = CUtf8Char;
#define CTEXT(str) CUTF8_TEXT(str)

#elif CCHAR_IS_UTF16

using CChar = CUtf16Char;
#define CTEXT(str) CUTF16_TEXT(str)

#elif CCHAR_IS_WCHAR

using CChar = CWideChar;
#define CTEXT(str) CWIDE_TEXT(str)

#else

#error "Unsupported CChar policy."

#endif

static_assert(sizeof(CAnsiChar) == 1, "CAnsiChar must be 1 byte.");
static_assert(sizeof(CUtf8Char) == 1, "CUtf8Char must be 1 byte.");
static_assert(sizeof(CUtf16Char) == 2, "CUtf16Char must be 2 bytes.");
static_assert(sizeof(CUtf32Char) == 4, "CUtf32Char must be 4 bytes.");

#if CCHAR_IS_UTF8

static_assert(sizeof(CChar) == 1, "CChar must be 1 byte in UTF-8 policy.");

#elif CCHAR_IS_UTF16

static_assert(sizeof(CChar) == 2, "CChar must be 2 bytes in UTF-16 policy.");

#elif CCHAR_IS_WCHAR

#if PLATFORM_WINDOWS
static_assert(sizeof(CChar) == 2, "CChar must be 2 bytes in Windows wchar policy.");
#endif

#endif
