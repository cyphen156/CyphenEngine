#pragma once

#include <string>

#include "Core/Public/CString.h"

// ============================================================================
// LinuxString
// ----------------------------------------------------------------------------
// Linux API 경계에서 사용하는 문자열 변환 유닛.
//
// CString은 빌드 타겟의 CChar 정책을 따릅니다.
// Linux / POSIX API는 char 문자열을 요구하므로,
// Linux 플랫폼 구현체는 API 호출 직전에 이 변환 유닛을 사용합니다.
//
// 현재 Linux 기본 정책이 UTF-8 CChar이면 대부분 복사만 수행합니다.
// 그래도 플랫폼 API 경계를 명확히 하기 위해 별도 계층으로 유지합니다.
//
// 책임:
//     CString을 Linux API용 std::string으로 변환합니다.
//     Linux 전용 문자열 리터럴 매크로를 제공합니다.
//
// ============================================================================

#define CLINUX_TEXT(str) str

class LinuxString final
{
public:
	static bool ToUtf8String(
		const CString& text,
		std::string& outText);

private:
	LinuxString() = delete;
	~LinuxString() = delete;

	LinuxString(const LinuxString& other) = delete;
	LinuxString& operator=(const LinuxString& other) = delete;

	LinuxString(LinuxString&& other) = delete;
	LinuxString& operator=(LinuxString&& other) = delete;
};
