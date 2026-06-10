#pragma once

#include <vector>

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/FileTypes.h"

// ============================================================================
// File
// ----------------------------------------------------------------------------
// CyphenEngine 저수준 단일 파일 I/O API.
//
// File은 UE의 FFileHelper에 가까운 공개 저수준 동기 I/O API입니다.
// File은 호출한 스레드에서 블로킹됩니다.
// 어느 스레드에서 실행할지는 호출자, TaskSystem, ResourceManager가 결정합니다.
//
// 책임:
//     단일 파일에서 raw byte를 읽습니다.
//     단일 파일에 raw byte를 씁니다.
//     단일 파일에서 text를 읽습니다.
//     단일 파일에 text를 씁니다.
//
// 반환 정책:
//     File 공개 API는 성공 / 실패만 반환합니다.
//     읽은 데이터는 out 파라미터에 저장합니다.
//     세부 실패 원인은 반환값으로 노출하지 않습니다.
//     실패 원인 기록은 Diagnostics / Logger / Platform diagnostics 계층의
//     책임입니다.
//
// Text:
//     Text API는 인코딩을 추측하지 않습니다.
//     TextEncoding을 받지 않는 오버로드는 CyphenEngine 기본 외부 텍스트
//     인코딩인 UTF-8을 사용합니다.
//     LineEnding을 받지 않는 WriteAllText 오버로드는 항상 LineEnding::LF를 사용합니다.
// ============================================================================

class File final
{
public:
	static bool ReadAllBytes(
		const CString& path,
		std::vector<uint8>& outBytes);

	static bool WriteAllBytes(
		const CString& path,
		const std::vector<uint8>& bytes);

	static bool ReadAllText(
		const CString& path,
		CString& outText);

	static bool ReadAllText(
		const CString& path,
		CString& outText,
		TextEncoding encoding);

	static bool WriteAllText(
		const CString& path,
		const CString& text);

	static bool WriteAllText(
		const CString& path,
		const CString& text,
		TextEncoding encoding);

	static bool WriteAllText(
		const CString& path,
		const CString& text,
		TextEncoding encoding,
		LineEnding lineEnding);

private:
	File() = delete;
	~File() = delete;

	File(const File& other) = delete;
	File& operator=(const File& other) = delete;

	File(File&& other) = delete;
	File& operator=(File&& other) = delete;
};
