#pragma once

#include "Core/Public/CString.h"

class PlatformFile;

// ============================================================================
// PlatformPath
// ----------------------------------------------------------------------------
// Path::Normalize가 만든 엔진 표준 path를 현재 빌드 타겟의 File IO 경계
// 규칙으로 직렬화하는 concrete HAL입니다.
//
// 책임:
//     엔진 표준 path를 플랫폼 path 규칙으로 변환합니다.
//     플랫폼 path 규칙을 엔진 표준 path로 변환합니다.
//
// 비책임:
//     OS API용 문자열 인코딩 전사(UTF-8 / UTF-16 등).
//     파일 / 디렉터리 I/O 실행.
//     파일 존재 확인 / 생성 / 삭제.
//     Win32 search pattern / glob / child path 조립.
//
// 주의:
//     outPlatformPath는 CString이지만 엔진 표준 path가 아닙니다.
//     Path API로 되먹이지 말고 PlatformFile 내부의 OS API 호출 직전까지만
//     짧게 사용합니다.
// ============================================================================

class PlatformPath final
{
public:
	static bool ToPlatformPath(const CString& enginePath, CString& outPlatformPath);

private:
	PlatformPath() = delete;
	~PlatformPath() = delete;

	PlatformPath(const PlatformPath& other) = delete;
	PlatformPath& operator=(const PlatformPath& other) = delete;

	PlatformPath(PlatformPath&& other) = delete;
	PlatformPath& operator=(PlatformPath&& other) = delete;
};
