#include "pch.h"

#include <algorithm>
#include <cwchar>
#include <limits>
#include <string>

#include "HAL/Private/PlatformFile.h"
#include "Platform/Windows/Private/WindowsPath.h"

namespace
{
	constexpr std::uintptr_t InvalidFileHandleValue =
		std::numeric_limits<std::uintptr_t>::max();

	HANDLE ToNativeHandle(FileHandle handle)
	{
		return reinterpret_cast<HANDLE>(handle.value);
	}

	bool DeleteDirectoryTree(const std::wstring& directoryName)
	{
		const std::wstring searchPattern = WindowsPath::JoinNativePath(directoryName, L"*");

		WIN32_FIND_DATAW findData = {};

		HANDLE findHandle = ::FindFirstFileW(searchPattern.c_str(), &findData);

		if (findHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		bool result = true;

		do
		{
			if (std::wcscmp(findData.cFileName, L".") == 0 ||
				std::wcscmp(findData.cFileName, L"..") == 0)
			{
				continue;
			}

			const std::wstring childPath = WindowsPath::JoinNativePath(directoryName, findData.cFileName);
			const bool isDirectory =
				(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			const bool isReparsePoint =
				(findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

			if (isDirectory)
			{
				if (isReparsePoint)
				{
					result = ::RemoveDirectoryW(childPath.c_str()) != FALSE;
				}
				else
				{
					result = DeleteDirectoryTree(childPath);
				}
			}
			else
			{
				result = ::DeleteFileW(childPath.c_str()) != FALSE;
			}
		} while (result && ::FindNextFileW(findHandle, &findData) != FALSE);

		if (result && ::GetLastError() != ERROR_NO_MORE_FILES)
		{
			result = false;
		}

		::FindClose(findHandle);

		return result && ::RemoveDirectoryW(directoryName.c_str()) != FALSE;
	}
}

bool PlatformFile::OpenRead(const CString& path, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::wstring fileName;

	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	HANDLE fileHandle = ::CreateFileW(
		fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	outHandle.value = reinterpret_cast<std::uintptr_t>(fileHandle);

	return true;
}

bool PlatformFile::OpenWrite(const CString& path, bool canReplace, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::wstring fileName;
	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	HANDLE fileHandle = ::CreateFileW(
		fileName.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		canReplace ? CREATE_ALWAYS : CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	outHandle.value = reinterpret_cast<std::uintptr_t>(fileHandle);

	return true;
}

bool PlatformFile::OpenAppend(const CString& path, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::wstring fileName;
	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	HANDLE fileHandle = ::CreateFileW(
		fileName.c_str(),
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	outHandle.value = reinterpret_cast<std::uintptr_t>(fileHandle);

	return true;
}

void PlatformFile::Close(FileHandle& handle)
{
	if (handle.value == InvalidFileHandleValue)
	{
		return;
	}

	::CloseHandle(ToNativeHandle(handle));

	handle.value = InvalidFileHandleValue;
}

bool PlatformFile::Read(FileHandle handle, uint8* outBytes, uint64 bytesToRead,
	uint64& outBytesRead)
{
	outBytesRead = 0;

	if (handle.value == InvalidFileHandleValue ||
		(outBytes == nullptr && bytesToRead > 0))
	{
		return false;
	}

	const DWORD readSize = static_cast<DWORD>(
		std::min<uint64>(
			bytesToRead,
			static_cast<uint64>(std::numeric_limits<DWORD>::max())));

	DWORD bytesRead = 0;

	if (::ReadFile(ToNativeHandle(handle), outBytes, readSize, &bytesRead, nullptr) == FALSE)
	{
		return false;
	}

	outBytesRead = static_cast<uint64>(bytesRead);

	return true;
}

bool PlatformFile::Write(FileHandle handle, const uint8* bytes, uint64 bytesToWrite,
	uint64& outBytesWritten)
{
	outBytesWritten = 0;

	if (handle.value == InvalidFileHandleValue ||
		(bytes == nullptr && bytesToWrite > 0))
	{
		return false;
	}

	const DWORD writeSize = static_cast<DWORD>(
		std::min<uint64>(
			bytesToWrite,
			static_cast<uint64>(std::numeric_limits<DWORD>::max())));

	DWORD bytesWritten = 0;

	if (::WriteFile(ToNativeHandle(handle), bytes, writeSize, &bytesWritten, nullptr) == FALSE)
	{
		return false;
	}

	outBytesWritten = static_cast<uint64>(bytesWritten);

	return true;
}

bool PlatformFile::Seek(FileHandle handle, uint64 position)
{
	if (handle.value == InvalidFileHandleValue)
	{
		return false;
	}

	LARGE_INTEGER targetPosition = {};
	targetPosition.QuadPart = static_cast<LONGLONG>(position);

	if (targetPosition.QuadPart < 0)
	{
		return false;
	}

	return ::SetFilePointerEx(
		ToNativeHandle(handle),
		targetPosition,
		nullptr,
		FILE_BEGIN) != FALSE;
}

bool PlatformFile::Tell(FileHandle handle, uint64& outPosition)
{
	outPosition = 0;

	if (handle.value == InvalidFileHandleValue)
	{
		return false;
	}

	LARGE_INTEGER distance = {};
	LARGE_INTEGER position = {};

	if (::SetFilePointerEx(
		ToNativeHandle(handle),
		distance,
		&position,
		FILE_CURRENT) == FALSE)
	{
		return false;
	}

	if (position.QuadPart < 0)
	{
		return false;
	}

	outPosition = static_cast<uint64>(position.QuadPart);

	return true;
}

bool PlatformFile::GetSize(FileHandle handle, uint64& outSize)
{
	outSize = 0;

	if (handle.value == InvalidFileHandleValue)
	{
		return false;
	}

	LARGE_INTEGER fileSize = {};

	if (::GetFileSizeEx(ToNativeHandle(handle), &fileSize) == FALSE ||
		fileSize.QuadPart < 0)
	{
		return false;
	}

	outSize = static_cast<uint64>(fileSize.QuadPart);

	return true;
}

bool PlatformFile::Create(const CString& path)
{
	FileHandle handle;

	if (!OpenWrite(path, false, handle))
	{
		return false;
	}

	Close(handle);

	return true;
}

bool PlatformFile::GetSize(const CString& path, uint64& outSize)
{
	outSize = 0;

	std::wstring fileName;
	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	WIN32_FILE_ATTRIBUTE_DATA fileData = {};

	if (::GetFileAttributesExW(
		fileName.c_str(),
		GetFileExInfoStandard,
		&fileData) == FALSE)
	{
		return false;
	}

	if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		return false;
	}

	outSize =
		(static_cast<uint64>(fileData.nFileSizeHigh) << 32) |
		static_cast<uint64>(fileData.nFileSizeLow);

	return true;
}

bool PlatformFile::Exists(const CString& path)
{
	std::wstring fileName;
	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	const DWORD attributes = ::GetFileAttributesW(fileName.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool PlatformFile::Remove(const CString& path)
{
	std::wstring fileName;
	if (!WindowsPath::ToNativePath(path, fileName))
	{
		return false;
	}

	return ::DeleteFileW(fileName.c_str()) != FALSE;
}

bool PlatformFile::DirectoryExists(const CString& path)
{
	std::wstring directoryName;

	if (!WindowsPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	const DWORD attributes = ::GetFileAttributesW(directoryName.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::MakeDirectory(const CString& path)
{
	std::wstring directoryName;

	if (!WindowsPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	if (::CreateDirectoryW(directoryName.c_str(), nullptr) != FALSE)
	{
		return true;
	}

	if (::GetLastError() != ERROR_ALREADY_EXISTS)
	{
		return false;
	}

	const DWORD attributes = ::GetFileAttributesW(directoryName.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::DeleteDirectory(const CString& path)
{
	std::wstring directoryName;

	if (!WindowsPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	return ::RemoveDirectoryW(directoryName.c_str()) != FALSE;
}

bool PlatformFile::DeleteDirectoryRecursively(const CString& path)
{
	std::wstring directoryName;

	if (!WindowsPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	return DeleteDirectoryTree(directoryName);
}

bool PlatformFile::Copy(const CString& sourcePath, const CString& targetPath,
	bool canReplace)
{
	std::wstring sourceFileName;
	std::wstring targetFileName;

	if (!WindowsPath::ToNativePath(sourcePath, sourceFileName) ||
		!WindowsPath::ToNativePath(targetPath, targetFileName))
	{
		return false;
	}

	return ::CopyFileW(
		sourceFileName.c_str(),
		targetFileName.c_str(),
		canReplace ? FALSE : TRUE) != FALSE;
}

bool PlatformFile::Move(const CString& sourcePath, const CString& targetPath,
	bool canReplace)
{
	std::wstring sourceFileName;
	std::wstring targetFileName;

	if (!WindowsPath::ToNativePath(sourcePath, sourceFileName) ||
		!WindowsPath::ToNativePath(targetPath, targetFileName))
	{
		return false;
	}

	DWORD flags = MOVEFILE_COPY_ALLOWED;

	if (canReplace)
	{
		flags |= MOVEFILE_REPLACE_EXISTING;
	}

	return ::MoveFileExW(
		sourceFileName.c_str(),
		targetFileName.c_str(),
		flags) != FALSE;
}
