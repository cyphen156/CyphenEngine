#pragma once

#include <string>

#include "Core/Public/CString.h"

// ============================================================================
// LinuxPath
// ----------------------------------------------------------------------------
// POSIX API에 넘길 native std::string 경로를 만드는 Linux 전용 facade.
// WindowsPath와 대칭. PlatformFile은 이 facade만 보고 PlatformPath/LinuxString
// 파이프라인을 모릅니다.
//
// 책임: engine path -> native std::string (ToNativePath), native join.
// 비책임: separator 정책(PlatformPath) / 전사(LinuxString) / I/O(PlatformFile).
// ============================================================================

class LinuxPath final
{
public:
	static bool ToNativePath(const CString& path, std::string& outPath);

	static std::string JoinNativePath(const std::string& parent, const char* child);

private:
	LinuxPath() = delete;
	~LinuxPath() = delete;

	LinuxPath(const LinuxPath& other) = delete;
	LinuxPath& operator=(const LinuxPath& other) = delete;

	LinuxPath(LinuxPath&& other) = delete;
	LinuxPath& operator=(LinuxPath&& other) = delete;
};
