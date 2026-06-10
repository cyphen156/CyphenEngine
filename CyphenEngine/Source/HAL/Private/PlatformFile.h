#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"

// ============================================================================
// PlatformFile
// ----------------------------------------------------------------------------
// File 공개 API 뒤에 숨겨지는 플랫폼 파일 I/O HAL입니다.
//
// PlatformFile은 파일 하나의 raw byte I/O를 실제 OS API로 수행합니다.
// Core의 File은 PlatformFile을 통해서만 플랫폼 파일 접근을 사용합니다.
//
// 책임:
//     플랫폼별 파일 열기.
//     플랫폼별 파일 byte 읽기.
//     플랫폼별 파일 byte 쓰기.
//     엔진 경로 문자열을 OS 호출용 경로로 변환.
//
// 반환 정책:
//     성공 / 실패만 반환합니다.
//     세부 OS 오류 정보는 이후 Diagnostics / Logger / Platform diagnostics
//     계층에서 기록합니다.
//
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

private:
	PlatformFile() = delete;
	~PlatformFile() = delete;

	PlatformFile(const PlatformFile& other) = delete;
	PlatformFile& operator=(const PlatformFile& other) = delete;

	PlatformFile(PlatformFile&& other) = delete;
	PlatformFile& operator=(PlatformFile&& other) = delete;
};
