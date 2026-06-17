#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"

// ============================================================================
// FileSystem
// ----------------------------------------------------------------------------
// CyphenEngine 저수준 파일 / 디렉터리 관리 API.
//
// File이 단일 파일 내용 I/O를 담당한다면, FileSystem은 파일 / 디렉터리의
// 존재 확인, 생성, 삭제, 크기 조회, 복사, 이동 같은 관리 연산을 담당합니다.
// File과 동일하게 호출한 스레드에서 블로킹됩니다.
//
// 책임:
//     파일 / 디렉터리 존재 확인.
//     파일 생성 / 삭제 / 크기 조회.
//     단일 디렉터리 생성 / 삭제.
//     디렉터리 트리 생성 / 명시적 재귀 삭제.
//     파일시스템 네임스페이스 복사 / 이동.
//
// 비책임:
//     프로젝트 루트 보정.
//     실행 파일 기준 경로 계산.
//     현재 작업 디렉터리 보정.
//     정책 경로(Resources / Saved / Logs / Temp) 보관.
//     AssetId / ResourceHandle / 비동기 로딩.
//
// 반환 정책:
//     성공 / 실패만 반환합니다.
//     세부 실패 원인은 Diagnostics 계층의 책임입니다.
//
// 주의:
//     FileSystem은 File에 의존하지 않습니다.
//     File과 FileSystem은 플랫폼 I/O 계층인 PlatformFile에 직접 의존합니다.
//     공개 API 이름은 Win32 매크로(DeleteFile / CreateDirectory / RemoveDirectory)와
//     충돌하지 않도록 RemoveFile / MakeDirectory / DeleteDirectory를 사용합니다.
// ============================================================================

class FileSystem final
{
public:
	static bool FileExists(const CString& path);
	static bool DirectoryExists(const CString& path);

	static bool Create(const CString& path);
	static bool GetSize(const CString& path, uint64& outSize);

	static bool RemoveFile(const CString& path);

	static bool MakeDirectory(const CString& path);
	static bool MakeDirectoryTree(const CString& path);

	static bool DeleteDirectory(const CString& path);
	static bool DeleteDirectoryRecursively(const CString& path);

	static bool Copy(const CString& sourcePath, const CString& targetPath);
	static bool Copy(const CString& sourcePath, const CString& targetPath,
		bool canReplace);

	static bool Move(const CString& sourcePath, const CString& targetPath);
	static bool Move(const CString& sourcePath, const CString& targetPath,
		bool canReplace);

private:
	FileSystem() = delete;
	~FileSystem() = delete;

	FileSystem(const FileSystem& other) = delete;
	FileSystem& operator=(const FileSystem& other) = delete;

	FileSystem(FileSystem&& other) = delete;
	FileSystem& operator=(FileSystem&& other) = delete;
};
