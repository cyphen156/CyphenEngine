#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/FileTypes.h"

// ============================================================================
// PlatformFile
// ----------------------------------------------------------------------------
// File / FileSystem 공개 API 뒤에 숨겨지는 플랫폼 파일 시스템 HAL.
//
// PlatformFile은 File / FileSystem의 플랫폼별 구현 상세입니다.
// 상위 엔진 계층과 모듈은 PlatformFile을 직접 호출하지 않고,
// File / FileSystem 공개 API를 통해 파일 시스템에 접근합니다.
//
// 책임:
//     raw 파일 open / close / read / write / seek / tell / size.
//     파일 하나의 생성 / 삭제 / 크기 조회.
//     디렉터리 하나의 존재 확인 / 생성 / 삭제(empty-only).
//     명시적 디렉터리 트리 삭제.
//     파일시스템 네임스페이스의 복사 / 이동.
//
// 비책임:
//     텍스트 인코딩.
//     줄바꿈 정책.
//     엔진 기본 경로 정책.
//     ResourceManager / Logger / Diagnostics.
//     텍스트 API.
//     FileStream RAII.
//     휴지통 이동 / undo-redo 트랜잭션.
//
// PlatformFile의 기본 대상은 파일입니다.
// 따라서 파일 단위 연산은 Create / Exists / Remove / GetSize처럼 짧게 쓰고,
// 디렉터리 연산만 Directory 이름을 명시합니다.
//
// PlatformFile은 native HANDLE / fd를 헤더에 노출하지 않습니다.
// FileHandle은 native 값을 담는 opaque 슬롯이며 RAII가 아닙니다.
// Close 책임은 호출자 또는 이후 FileStream RAII 계층이 집니다.
//
// 인터페이스(virtual)가 아니라, 플랫폼별 동일 시그니처 구현을 빌드가 선택하는
// concrete HAL입니다.
// 호출 권한은 File / FileSystem으로 한정합니다(friend, 전부 private).
//
// 반환 정책:
//     성공 / 실패만 반환합니다.
//     세부 OS 오류 정보는 이후 Diagnostics / Logger / Platform diagnostics
//     계층에서 기록합니다.
// ============================================================================

class File;
class FileSystem;

class PlatformFile final
{
private:
	friend class File;
	friend class FileSystem;

	static bool OpenRead(const CString& path, FileHandle& outHandle);
	static bool OpenWrite(const CString& path, bool canReplace, FileHandle& outHandle);
	static bool OpenAppend(const CString& path, FileHandle& outHandle);
	static void Close(FileHandle& handle);

	static bool Read(FileHandle handle, uint8* outBytes, uint64 bytesToRead, uint64& outBytesRead);
	static bool Write(FileHandle handle, const uint8* bytes, uint64 bytesToWrite, uint64& outBytesWritten);
	static bool Seek(FileHandle handle, uint64 position);
	static bool Tell(FileHandle handle, uint64& outPosition);
	static bool GetSize(FileHandle handle, uint64& outSize);

	static bool Create(const CString& path);
	static bool GetSize(const CString& path, uint64& outSize);

	static bool Exists(const CString& path);
	static bool Remove(const CString& path);

	static bool DirectoryExists(const CString& path);
	static bool MakeDirectory(const CString& path);
	static bool DeleteDirectory(const CString& path);
	static bool DeleteDirectoryRecursively(const CString& path);

	static bool Copy(const CString& sourcePath, const CString& targetPath,
		bool canReplace);

	static bool Move(const CString& sourcePath, const CString& targetPath,
		bool canReplace);

	PlatformFile() = delete;
	~PlatformFile() = delete;

	PlatformFile(const PlatformFile& other) = delete;
	PlatformFile& operator=(const PlatformFile& other) = delete;

	PlatformFile(PlatformFile&& other) = delete;
	PlatformFile& operator=(PlatformFile&& other) = delete;
};
