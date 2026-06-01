#pragma once

#include <string>

#include "Core/Public/CChar.h"

// ============================================================================
// CString
// ----------------------------------------------------------------------------
// CyphenEngine default string type.
//
// CString is not a class wrapper.
// CString is std::basic_string<CChar> based on the current CChar policy.
//
// CCHAR_IS_UTF8:
//     CString = std::basic_string<char>
//
// CCHAR_IS_UTF16:
//     CString = std::basic_string<char16_t>
//
// Rules:
//     Engine string literals must use CTEXT("...").
//     CString::length / find / substr / operator[] are CChar code unit based.
//     Platform string conversion belongs to Platform or Converter layer.
// ============================================================================

using CString = std::basic_string<CChar>;
