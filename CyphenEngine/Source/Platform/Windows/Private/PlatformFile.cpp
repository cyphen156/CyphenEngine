#include "pch.h"

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "HAL/Private/PlatformFile.h"
#include "Platform/Windows/Private/WindowsString.h"

namespace
{
	bool ReadAllBytesFromHandle(
		HANDLE fileHandle,
		std::vector<uint8>& outBytes)
	{
		LARGE_INTEGER fileSize = {};

		if (::GetFileSizeEx(fileHandle, &fileSize) == FALSE)
		{
			return false;
		}

		if (fileSize.QuadPart < 0)
		{
			return false;
		}

		if (static_cast<unsigned long long>(fileSize.QuadPart) >
			static_cast<unsigned long long>(std::numeric_limits<std::size_t>::max()))
		{
			return false;
		}

		outBytes.clear();
		outBytes.resize(static_cast<std::size_t>(fileSize.QuadPart));

		std::size_t offset = 0;
		std::size_t remainingSize = outBytes.size();

		while (remainingSize > 0)
		{
			const DWORD readSize = static_cast<DWORD>(
				std::min<std::size_t>(
					remainingSize,
					static_cast<std::size_t>(std::numeric_limits<DWORD>::max())));

			DWORD bytesRead = 0;

			const BOOL bResult = ::ReadFile(
				fileHandle,
				outBytes.data() + offset,
				readSize,
				&bytesRead,
				nullptr);

			if (bResult == FALSE)
			{
				outBytes.clear();
				return false;
			}

			if (bytesRead == 0)
			{
				break;
			}

			offset += bytesRead;
			remainingSize -= bytesRead;
		}

		if (offset != outBytes.size())
		{
			outBytes.clear();
			return false;
		}

		return true;
	}

	bool WriteAllBytesToHandle(
		HANDLE fileHandle,
		const std::vector<uint8>& bytes)
	{
		std::size_t offset = 0;
		std::size_t remainingSize = bytes.size();

		while (remainingSize > 0)
		{
			const DWORD writeSize = static_cast<DWORD>(
				std::min<std::size_t>(
					remainingSize,
					static_cast<std::size_t>(std::numeric_limits<DWORD>::max())));

			DWORD bytesWritten = 0;

			const BOOL bResult = ::WriteFile(
				fileHandle,
				bytes.data() + offset,
				writeSize,
				&bytesWritten,
				nullptr);

			if (bResult == FALSE)
			{
				return false;
			}

			if (bytesWritten != writeSize)
			{
				return false;
			}

			offset += bytesWritten;
			remainingSize -= bytesWritten;
		}

		return true;
	}
}

bool PlatformFile::ReadAllBytes(
	const CString& path,
	std::vector<uint8>& outBytes)
{
	outBytes.clear();

	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	HANDLE fileHandle = ::CreateFileW(
		fileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	const bool bResult = ReadAllBytesFromHandle(fileHandle, outBytes);

	::CloseHandle(fileHandle);

	return bResult;
}

bool PlatformFile::WriteAllBytes(
	const CString& path,
	const std::vector<uint8>& bytes)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	HANDLE fileHandle = ::CreateFileW(
		fileName.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	const bool bResult = WriteAllBytesToHandle(fileHandle, bytes);

	::CloseHandle(fileHandle);

	return bResult;
}

bool PlatformFile::FileExists(const CString& path)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
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

bool PlatformFile::DirectoryExists(const CString& path)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	const DWORD attributes = ::GetFileAttributesW(fileName.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::RemoveFile(const CString& path)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	return ::DeleteFileW(fileName.c_str()) != FALSE;
}

bool PlatformFile::MakeDirectory(const CString& path)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	if (::CreateDirectoryW(fileName.c_str(), nullptr) != FALSE)
	{
		return true;
	}

	if (::GetLastError() != ERROR_ALREADY_EXISTS)
	{
		return false;
	}

	// 이미 존재하면 디렉터리일 때만 성공으로 취급합니다.
	// 같은 이름의 파일이 있으면 실패입니다.
	const DWORD attributes = ::GetFileAttributesW(fileName.c_str());

	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::DeleteDirectory(const CString& path)
{
	if (path.empty())
	{
		return false;
	}

	std::wstring fileName;

	if (!WindowsString::ToWideString(path, fileName))
	{
		return false;
	}

	// RemoveDirectoryW는 빈 디렉터리만 삭제합니다.
	// 비어있지 않으면 실패합니다.
	return ::RemoveDirectoryW(fileName.c_str()) != FALSE;
}
