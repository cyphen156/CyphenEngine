#pragma once

#include "Core/Public/CString.h"

// ============================================================================
// Path
// ----------------------------------------------------------------------------
// 플랫폼 간 경로 문자열 규칙을 엔진 공통 형식으로 다루기 위한 Core 경로 클래스입니다.
//
// Path는 파일 시스템에 직접 접근하지 않습니다.
// Path는 프로젝트 루트, Content, Saved, Logs 같은 엔진 의미 경로를 보관하지 않습니다.
// Path는 OS 현재 working directory 같은 실행 환경 상태도 조회하지 않습니다.
//
// 이 클래스는 외부에서 전달받은 경로 문자열을 조작하고 판별하는
// 무상태 정적 경로 유틸리티입니다.
//
// 책임:
//   - 경로 결합
//   - 경로 구분자를 엔진 내부 표준 형식으로 정규화
//   - 명시된 기준 경로를 기준으로 절대 경로 / 상대 경로 변환
//   - 디렉터리명 / 파일명 / 확장자 추출
//   - 절대 경로 / 상대 경로 판별
//
// 비책임:
//   - 프로젝트 루트 경로 보관
//   - OS 현재 working directory 조회
//   - 파일 존재 검사
//   - 파일 읽기 / 쓰기
//   - 디렉터리 생성 / 삭제
//   - Content / Saved / Logs / Config 같은 엔진 정책 경로 제공
//   - Resource / Asset 로딩 정책
//   - OS working directory 변경
//
// 주의:
//   Path의 내부 표준 경로 구분자는 '/'입니다.
//   플랫폼 네이티브 경로 변환은 추후 PlatformPath 또는 FileSystem 계층에서 처리합니다.
// ============================================================================

class Path final
{
public:
	static CString Combine(const CString& left, const CString& right);
	static CString Normalize(const CString& path);

	static CString ToAbsolute(const CString& path, const CString& basePath);
	static CString ToRelative(const CString& path, const CString& basePath);

	static CString GetDirectoryName(const CString& path);
	static CString GetFileName(const CString& path);
	static CString GetFileNameWithoutExtension(const CString& path);
	static CString GetExtension(const CString& path);

	static bool IsAbsolute(const CString& path);
	static bool IsRelative(const CString& path);

private:
	Path() = delete;
	~Path() = delete;

	Path(const Path& other) = delete;
	Path& operator=(const Path& other) = delete;

	Path(Path&& other) = delete;
	Path& operator=(Path&& other) = delete;
};
