#include "pch.h"

#include <limits>

#include "Core/Public/File.h"
#include "Core/Public/FileStream.h"

namespace
{
	constexpr std::uintptr_t InvalidFileHandleValue =
		std::numeric_limits<std::uintptr_t>::max();
}

FileStream::FileStream(FileHandle handle)
	: handle(handle)
{
}

FileStream::~FileStream()
{
	Close();
}

FileStream::FileStream(FileStream&& other) noexcept
	: handle(other.handle)
{
	other.handle.value = InvalidFileHandleValue;
}

FileStream& FileStream::operator=(FileStream&& other) noexcept
{
	if (this != &other)
	{
		Close();

		handle = other.handle;
		other.handle.value = InvalidFileHandleValue;
	}

	return *this;
}

bool FileStream::OpenRead(const CString& path, FileStream& outStream)
{
	FileHandle openedHandle;

	if (!File::OpenRead(path, openedHandle))
	{
		return false;
	}

	outStream = FileStream(openedHandle);

	return true;
}

bool FileStream::OpenWrite(const CString& path, bool canReplace, FileStream& outStream)
{
	FileHandle openedHandle;

	if (!File::OpenWrite(path, canReplace, openedHandle))
	{
		return false;
	}

	outStream = FileStream(openedHandle);

	return true;
}

bool FileStream::OpenAppend(const CString& path, FileStream& outStream)
{
	FileHandle openedHandle;

	if (!File::OpenAppend(path, openedHandle))
	{
		return false;
	}

	outStream = FileStream(openedHandle);

	return true;
}

bool FileStream::IsOpen() const
{
	return handle.value != InvalidFileHandleValue;
}

void FileStream::Close()
{
	File::Close(handle);
}

bool FileStream::Read(uint8* outBytes, uint64 bytesToRead, uint64& outBytesRead)
{
	return File::Read(handle, outBytes, bytesToRead, outBytesRead);
}

bool FileStream::Write(const uint8* bytes, uint64 bytesToWrite, uint64& outBytesWritten)
{
	return File::Write(handle, bytes, bytesToWrite, outBytesWritten);
}

bool FileStream::Seek(uint64 position)
{
	return File::Seek(handle, position);
}

bool FileStream::Tell(uint64& outPosition) const
{
	return File::Tell(handle, outPosition);
}

bool FileStream::GetSize(uint64& outSize) const
{
	return File::GetSize(handle, outSize);
}
