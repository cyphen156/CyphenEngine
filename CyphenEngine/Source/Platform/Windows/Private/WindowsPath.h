#pragma once

#include <string>

#include "Core/Public/CString.h"

// ============================================================================
// WindowsPath
// ----------------------------------------------------------------------------
// Win32 API에 넘길 native std::wstring 경로를 만드는 Windows 전용 facade.
//
// PlatformFile은 이 facade만 보고, CString 경로 정책(PlatformPath)과
// 문자열 전사(WindowsString) 파이프라인을 알 필요가 없습니다.
//
// 책임:
//     engine path -> native std::wstring path (ToNativePath).
//     native std::wstring 경로 조립 (JoinNativePath).
//     long-path \\?\ / UNC \\server\share / drive-root C:\ 같은
//     Windows native 경로 의미론 (향후 확장 거처).
//
// 비책임:
//     CString separator 정책        -> PlatformPath.
//     CString <-> wstring 전사       -> WindowsString.
//     파일 I/O / 열거 / 삭제 정책    -> PlatformFile.
//
// WindowsString과 대칭인 platform-private 부품입니다(공유 HAL 계약 아님).
// ============================================================================

class WindowsPath final
{
public:
	static bool ToNativePath(const CString& path, std::wstring& outPath);

	static std::wstring JoinNativePath(const std::wstring& parent, const wchar_t* child);

private:
	WindowsPath() = delete;
	~WindowsPath() = delete;

	WindowsPath(const WindowsPath& other) = delete;
	WindowsPath& operator=(const WindowsPath& other) = delete;

	WindowsPath(WindowsPath&& other) = delete;
	WindowsPath& operator=(WindowsPath&& other) = delete;
};
