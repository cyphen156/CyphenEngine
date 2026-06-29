#pragma once

#include "Core/Public/CPrimitiveTypes.h"
#include "Core/Public/CString.h"
#include "Core/Public/FileTypes.h"

// ============================================================================
// FileStream
// ----------------------------------------------------------------------------
// FileHandle을 소유하는 RAII 파일 스트림.
//
// FileHandle은 raw non-RAII I/O 프리미티브이고, FileStream은 그 핸들을 소유해
// Close를 보장하는 안전한 사용면입니다.
// ============================================================================

class FileStream final
{
public:
	FileStream() = default;
	~FileStream();

	FileStream(FileStream&& other) noexcept;
	FileStream& operator=(FileStream&& other) noexcept;

	FileStream(const FileStream& other) = delete;
	FileStream& operator=(const FileStream& other) = delete;

	static bool OpenRead(const CString& path, FileStream& outStream);
	static bool OpenWrite(const CString& path, bool canReplace, FileStream& outStream);
	static bool OpenAppend(const CString& path, FileStream& outStream);

	bool IsOpen() const;
	void Close();

	bool Read(uint8* outBytes, uint64 bytesToRead, uint64& outBytesRead);
	bool Write(const uint8* bytes, uint64 bytesToWrite, uint64& outBytesWritten);
	bool Seek(uint64 position);
	bool Tell(uint64& outPosition) const;
	bool GetSize(uint64& outSize) const;

private:
	explicit FileStream(FileHandle handle);

	FileHandle handle;
};
