#pragma once

#include "Core/Public/CString.h"

// ============================================================================
// Path
// ----------------------------------------------------------------------------
// 엔진 내부 표준 경로 문자열을 조작하기 위한 Core 경로 유틸리티입니다.
//
// Path는 파일 시스템에 직접 접근하지 않습니다.
// Path는 프로젝트 루트, Content, Saved, Logs 같은 엔진 의미 경로를 보관하지 않습니다.
// Path는 OS 현재 working directory 같은 실행 환경 상태도 조회하지 않습니다.
// Path는 OS API 호출용 native path를 생성하지 않습니다.
//
// 이 클래스는 CString 문자열을 대상으로 엔진 경로 규칙을 적용하는
// 무상태 정적 경로 유틸리티입니다.
//
// 책임:
//   - 경로 결합
//   - 입력 경로 문자열을 엔진 내부 표준 형식으로 정규화
//   - 디렉터리명 / 파일명 / 확장자 추출
//
// 비책임:
//   - OS API 호출용 native 경로 생성
//   - OS native path의 절대 / 상대 규칙 해석
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
//   Path의 엔진 표준 경로 구분자는 '/'입니다.
//
//   Path::Normalize는 명시적 변환 함수입니다.
//   Normalize는 CyphenEngine Path 입력 문법에서 호환 구분자로 허용하는 '\'
//   그리고 엔진 표준 구분자 '/'를 모두 엔진 표준 구분자 '/'로 맞춥니다.
//   또한 연속된 경로 구분자를 하나의 '/'로 축약합니다.
//
//   Combine / GetDirectoryName / GetFileName / GetExtension 계열은
//   입력 문자열을 정규화하지 않습니다.
//   이 함수들은 엔진 표준 구분자 '/'만 경로 구분자로 보고 문자열을 처리합니다.
//
//   OS native path 변환은 File / FileSystem의 플랫폼 구현 계층에서 처리합니다.
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
