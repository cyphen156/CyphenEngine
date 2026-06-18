#pragma once

#include <string>

#include "Core/Public/CChar.h"

// ============================================================================
// CString
// ----------------------------------------------------------------------------
// CyphenEngine 내부 문자열 타입.
//
// CString은 별도 문자열 wrapper가 아닙니다.
// std::basic_string<CChar> 별칭입니다.
//
// CString은 문자열을 저장하는 컨테이너이며,
// 인코딩 변환이나 글자 단위 처리를 직접 수행하지 않습니다.
//
// CChar 정책:
//     Build / Framework 계층에서 빌드 타겟별로 결정합니다.
//
// 주의:
//     CString::length()는 사람이 보는 글자 수가 아닙니다.
//     CString::length()는 CChar 코드 유닛 개수입니다.
// ============================================================================

using CString = std::basic_string<CChar>;
