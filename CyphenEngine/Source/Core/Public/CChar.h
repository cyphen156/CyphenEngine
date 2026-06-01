#pragma once

// ============================================================================
// CChar
// ----------------------------------------------------------------------------
// CyphenEngine character code unit policy.
//
// CChar is the code unit type used by CString.
// Exactly one policy must be selected.
//
// Default:
//     CCHAR_IS_UTF8  = 1
//     CCHAR_IS_UTF16 = 0
//
// Rules:
//     CChar is not a user-visible character.
//     CString indexing is based on CChar code units.
//     Human-readable text processing belongs to CText or text search layer.
// ============================================================================

using CAnsiChar = char;
using CUtf8Char = char;
using CUtf16Char = char16_t;
using CUtf32Char = char32_t;

#define CANSI_TEXT(str) str
#define CUTF8_TEXT(str) u8##str
#define CUTF16_TEXT(str) u##str
#define CUTF32_TEXT(str) U##str

#ifndef CCHAR_IS_UTF8
#define CCHAR_IS_UTF8 1
#endif

#ifndef CCHAR_IS_UTF16
#define CCHAR_IS_UTF16 0
#endif

#if CCHAR_IS_UTF8 == CCHAR_IS_UTF16
#error "Exactly one CChar policy must be selected."
#endif

#if CCHAR_IS_UTF8

using CChar = CUtf8Char;

#define CTEXT(str) CUTF8_TEXT(str)

#elif CCHAR_IS_UTF16

using CChar = CUtf16Char;

#define CTEXT(str) CUTF16_TEXT(str)

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

#endif
