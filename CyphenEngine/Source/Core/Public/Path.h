#pragma once

#include "Core/Public/CString.h"
#include "Core/Public/Separator.h"

// ============================================================================
// Path
// ----------------------------------------------------------------------------
// 엔진 내부 표준 경로 문자열을 다루는 Core 경로 유틸리티입니다.
//
// Path는 파일 시스템에 직접 접근하지 않습니다.
// OS native path 변환은 File / FileSystem의 플랫폼 구현 계층에서 처리합니다.
//
// 경로 구분자 판정은 Separators 정책을 사용합니다.
// Normalize는 등록된 구분자를 엔진 구분자로 통일하고, 연속 구분자를 하나로
// 압축합니다.
// ============================================================================

class Path final
{
public:
	static CString Combine(const CString& left, const CString& right);
	static CString Normalize(const CString& path);

	static CString GetDirectoryName(const CString& path);
	static CString GetFileName(const CString& path);
	static CString GetFileNameWithoutExtension(const CString& path);
	static CString GetExtension(const CString& path);

private:
	Path() = delete;
	~Path() = delete;

	Path(const Path& other) = delete;
	Path& operator=(const Path& other) = delete;

	Path(Path&& other) = delete;
	Path& operator=(Path&& other) = delete;
};
