#pragma once

#include <string>

#include "Core/Public/CString.h"

// ============================================================================
// WindowsString
// ----------------------------------------------------------------------------
// Windows API 경계에서 사용하는 문자열 변환 유닛.
//
// CString은 빌드 타겟의 CChar 정책을 따릅니다.
// Windows API는 wchar_t 문자열을 요구하므로,
// Windows 플랫폼 구현체는 API 호출 직전에 이 변환 유닛을 사용합니다.
//
// 책임:
//     CString을 Windows API용 std::wstring으로 변환합니다.
//     Windows 전용 문자열 리터럴 매크로를 제공합니다.
//
// ============================================================================

#define CWINDOWS_TEXT(str) L##str

class WindowsString final
{
public:
	static bool ToWideString(
		const CString& text,
		std::wstring& outText);

private:
	WindowsString() = delete;
	~WindowsString() = delete;

	WindowsString(const WindowsString& other) = delete;
	WindowsString& operator=(const WindowsString& other) = delete;

	WindowsString(WindowsString&& other) = delete;
	WindowsString& operator=(WindowsString&& other) = delete;
};
