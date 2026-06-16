#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"

// ============================================================================
// PlatformFile
// ----------------------------------------------------------------------------
// File / FileSystem 공개 API 뒤에 숨겨지는 플랫폼 파일 시스템 HAL.
//
// 이름은 File이지만, 파일 내용 byte I/O와 파일 시스템 네임스페이스 관리
// (존재 확인·삭제·디렉터리 생성/삭제)를 함께 담당합니다.
// 인터페이스(virtual)가 아니라, 플랫폼별 동일 시그니처 구현을 빌드가 선택하는
// concrete HAL입니다.
//
// 반환 정책:
//     성공 / 실패만 반환합니다.
//     세부 OS 오류 정보는 이후 Diagnostics / Logger / Platform diagnostics
//     계층에서 기록합니다.
// ============================================================================

class PlatformFile final
{
public:
	static bool ReadAllBytes(
		const CString& path,
		std::vector<uint8>& outBytes);

	static bool WriteAllBytes(
		const CString& path,
		const std::vector<uint8>& bytes);

	static bool FileExists(const CString& path);
	static bool DirectoryExists(const CString& path);

	static bool RemoveFile(const CString& path);

	static bool MakeDirectory(const CString& path);
	static bool DeleteDirectory(const CString& path);

private:
	PlatformFile() = delete;
	~PlatformFile() = delete;

	PlatformFile(const PlatformFile& other) = delete;
	PlatformFile& operator=(const PlatformFile& other) = delete;

	PlatformFile(PlatformFile&& other) = delete;
	PlatformFile& operator=(PlatformFile&& other) = delete;
};
