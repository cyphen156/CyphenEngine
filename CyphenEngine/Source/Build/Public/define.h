#pragma once

// ============================================================================
// Define
// ----------------------------------------------------------------------------
// CyphenEngine 공통 빌드 정책.
//
// PlatformDefine.h:
//     TARGET_PLATFORM_* -> PLATFORM_* 확정
//
// framework.h:
//     플랫폼 시스템 헤더 및 OS 타입 준비
//
// define.h:
//     플랫폼과 무관한 엔진 공통 빌드 정책 확정
//
// CChar 정책:
//     CChar 정책은 Core가 결정하지 않습니다.
//     빌드 설정 또는 define.h에서 결정합니다.
//     명시적인 CCHAR 정책이 없으면 UTF-8을 기본값으로 사용합니다.
//
// 주의:
//     CChar 정책은 런타임 옵션이 아닙니다.
//     빌드 산출물의 문자열 ABI입니다.
//     같은 실행파일 / 같은 모듈 묶음은 반드시 같은 정책으로 빌드해야 합니다.
// ============================================================================

// ============================================================================
// CChar Policy
// ============================================================================

#if !defined(CCHAR_IS_UTF8) && !defined(CCHAR_IS_UTF16) && !defined(CCHAR_IS_WCHAR)

#define CCHAR_IS_UTF8 1
#define CCHAR_IS_UTF16 0
#define CCHAR_IS_WCHAR 0

#endif

#ifndef CCHAR_IS_UTF8
#define CCHAR_IS_UTF8 0
#endif

#ifndef CCHAR_IS_UTF16
#define CCHAR_IS_UTF16 0
#endif

#ifndef CCHAR_IS_WCHAR
#define CCHAR_IS_WCHAR 0
#endif

#if (CCHAR_IS_UTF8 + CCHAR_IS_UTF16 + CCHAR_IS_WCHAR) != 1
#error "Exactly one CCHAR_IS_* policy must be selected."
#endif

#if CCHAR_IS_WCHAR && !PLATFORM_WINDOWS
#error "CCHAR_IS_WCHAR is currently supported only for Windows targets."
#endif
