#include "pch.h"

#include <limits>
#include <vector>

#include "Core/Public/File.h"
#include "Core/Public/TextCodec.h"
#include "HAL/Private/PlatformFile.h"

bool File::ReadAllBytes(const CString& path, std::vector<uint8>& outBytes)
{
	outBytes.clear();

	FileHandle handle;

	if (!OpenRead(path, handle))
	{
		return false;
	}

	bool result = false;
	uint64 fileSize = 0;

	if (GetSize(handle, fileSize) &&
		fileSize <= static_cast<uint64>(std::numeric_limits<std::size_t>::max()))
	{
		outBytes.resize(static_cast<std::size_t>(fileSize));

		uint64 offset = 0;
		result = true;

		while (offset < fileSize)
		{
			uint64 bytesRead = 0;

			if (!Read(handle, outBytes.data() + offset, fileSize - offset, bytesRead) ||
				bytesRead == 0)
			{
				result = false;
				break;
			}

			offset += bytesRead;
		}

		if (offset != fileSize)
		{
			result = false;
		}
	}

	Close(handle);

	if (!result)
	{
		outBytes.clear();
	}

	return result;
}

bool File::WriteAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	FileHandle handle;

	if (!OpenWrite(path, true, handle))
	{
		return false;
	}

	bool result = true;
	uint64 offset = 0;
	const uint64 byteCount = static_cast<uint64>(bytes.size());

	while (offset < byteCount)
	{
		uint64 bytesWritten = 0;

		if (!Write(handle, bytes.data() + offset, byteCount - offset, bytesWritten) ||
			bytesWritten == 0)
		{
			result = false;
			break;
		}

		offset += bytesWritten;
	}

	Close(handle);

	return result;
}

bool File::AppendAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	FileHandle handle;

	if (!OpenAppend(path, handle))
	{
		return false;
	}

	bool result = true;
	uint64 offset = 0;
	const uint64 byteCount = static_cast<uint64>(bytes.size());

	while (offset < byteCount)
	{
		uint64 bytesWritten = 0;

		if (!Write(handle, bytes.data() + offset, byteCount - offset, bytesWritten) ||
			bytesWritten == 0)
		{
			result = false;
			break;
		}

		offset += bytesWritten;
	}

	Close(handle);

	return result;
}

bool File::OpenRead(const CString& path, FileHandle& outHandle)
{
	return PlatformFile::OpenRead(path, outHandle);
}

bool File::OpenWrite(const CString& path, bool canReplace, FileHandle& outHandle)
{
	return PlatformFile::OpenWrite(path, canReplace, outHandle);
}

bool File::OpenAppend(const CString& path, FileHandle& outHandle)
{
	return PlatformFile::OpenAppend(path, outHandle);
}

void File::Close(FileHandle& handle)
{
	PlatformFile::Close(handle);
}

bool File::Read(FileHandle handle, uint8* outBytes, uint64 bytesToRead,
	uint64& outBytesRead)
{
	return PlatformFile::Read(handle, outBytes, bytesToRead, outBytesRead);
}

bool File::Write(FileHandle handle, const uint8* bytes, uint64 bytesToWrite,
	uint64& outBytesWritten)
{
	return PlatformFile::Write(handle, bytes, bytesToWrite, outBytesWritten);
}

bool File::Seek(FileHandle handle, uint64 position)
{
	return PlatformFile::Seek(handle, position);
}

bool File::Tell(FileHandle handle, uint64& outPosition)
{
	return PlatformFile::Tell(handle, outPosition);
}

bool File::GetSize(FileHandle handle, uint64& outSize)
{
	return PlatformFile::GetSize(handle, outSize);
}

bool File::ReadAllText(const CString& path, CString& outText)
{
	return ReadAllText(
		path,
		outText,
		TextCodec::GetDefaultTextEncoding());
}

bool File::ReadAllText(const CString& path, CString& outText,
	TextEncoding encoding)
{
	std::vector<uint8> bytes;

	if (!ReadAllBytes(path, bytes))
	{
		outText.clear();
		return false;
	}

	if (!TextCodec::Decode(bytes, outText, encoding))
	{
		outText.clear();
		return false;
	}

	return true;
}

bool File::WriteAllText(const CString& path, const CString& text)
{
	return WriteAllText(
		path,
		text,
		TextCodec::GetDefaultTextEncoding(),
		LineEnding::LF);
}

bool File::WriteAllText(const CString& path, const CString& text,
	TextEncoding encoding)
{
	return WriteAllText(
		path,
		text,
		encoding,
		LineEnding::LF);
}

bool File::WriteAllText(const CString& path, const CString& text,
	TextEncoding encoding, LineEnding lineEnding)
{
	std::vector<uint8> bytes;

	if (!TextCodec::Encode(text, bytes, encoding, lineEnding))
	{
		return false;
	}

	return WriteAllBytes(path, bytes);
}

bool File::AppendAllText(const CString& path, const CString& text)
{
	TextEncoding encoding = TextCodec::GetDefaultTextEncoding();

	uint64 fileSize = 0;

	if (PlatformFile::GetSize(path, fileSize) && fileSize > 0)
	{
		FileHandle handle;
		std::vector<uint8> headBytes;

		if (OpenRead(path, handle))
		{
			headBytes.resize(3);

			uint64 bytesRead = 0;

			if (Read(handle, headBytes.data(), 3, bytesRead))
			{
				headBytes.resize(static_cast<std::size_t>(bytesRead));
				encoding = TextCodec::ResolveEncoding(headBytes, encoding);
			}

			Close(handle);
		}
	}

	std::vector<uint8> bytes;

	if (!TextCodec::Encode(text, bytes, encoding, LineEnding::Preserve))
	{
		return false;
	}

	return AppendAllBytes(path, bytes);
}
