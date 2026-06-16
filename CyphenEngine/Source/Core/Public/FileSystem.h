#pragma once

#include "Core/Public/CString.h"

// ============================================================================
// FileSystem
// ----------------------------------------------------------------------------
// CyphenEngine 저수준 파일/디렉터리 관리 API.
//
// File이 단일 파일 내용 I/O를 담당한다면, FileSystem은 파일/디렉터리의
// 존재 확인·삭제·디렉터리 생성/삭제 같은 관리 연산을 담당합니다.
// File과 동일하게 호출한 스레드에서 블로킹됩니다.
//
// 책임:
//     파일/디렉터리 존재 확인.
//     단일 파일 삭제.
//     단일 디렉터리 생성 / 삭제.
//
// 비책임:
//     주어진 경로만 처리합니다. 프로젝트 루트 보관·실행 파일 기준 경로 계산·
//     현재 작업 디렉터리 보정·정책 경로(Resources/Saved/Logs/Temp) 보관을 하지
//     않습니다. 이는 이후 RuntimePath 계층의 책임입니다.
//     파일 내용 읽기 / 쓰기는 File 책임입니다.
//     재귀 삭제·복사·이동은 이번 범위에서 제외합니다.
//     AssetId / ResourceHandle / 비동기 로딩은 ResourceManager 계층의 책임입니다.
//
// 반환 정책:
//     성공 / 실패만 반환합니다. 세부 실패 원인은 Diagnostics 계층의 책임입니다.
//     MakeDirectory는 대상이 이미 디렉터리면 성공으로 취급합니다(멱등).
//     DeleteDirectory는 빈 디렉터리만 삭제합니다.
//
// 주의:
//     FileSystem은 File을 소비하지 않습니다.
//     File과 FileSystem은 플랫폼 I/O 계층(PlatformFile)을 직접 소비하는 형제입니다.
//     공개 API 이름은 Win32 매크로(DeleteFile / CreateDirectory / RemoveDirectory)
//     와 충돌하지 않도록 RemoveFile / MakeDirectory / DeleteDirectory를 사용합니다.
// ============================================================================

class FileSystem final
{
public:
	static bool FileExists(const CString& path);
	static bool DirectoryExists(const CString& path);

	static bool RemoveFile(const CString& path);

	static bool MakeDirectory(const CString& path);
	static bool DeleteDirectory(const CString& path);

private:
	FileSystem() = delete;
	~FileSystem() = delete;

	FileSystem(const FileSystem& other) = delete;
	FileSystem& operator=(const FileSystem& other) = delete;

	FileSystem(FileSystem&& other) = delete;
	FileSystem& operator=(FileSystem&& other) = delete;
};
