#include "pch.h"

#include <algorithm>
#include <cwchar>
#include <limits>
#include <string>
#include <vector>

#include "Core/Public/Separator.h"
#include "HAL/Private/PlatformFile.h"
#include "Platform/Windows/Private/WindowsString.h"

namespace
{
	class ScopedFileHandle final
	{
	public:
		explicit ScopedFileHandle(HANDLE fileHandle)
			: fileHandle(fileHandle)
		{
		}

		~ScopedFileHandle()
		{
			Close();
		}

		ScopedFileHandle(ScopedFileHandle&& other) noexcept
			: fileHandle(other.fileHandle)
		{
			other.fileHandle = INVALID_HANDLE_VALUE;
		}

		ScopedFileHandle& operator=(ScopedFileHandle&& other) noexcept
		{
			if (this != &other)
			{
				Close();

				fileHandle = other.fileHandle;
				other.fileHandle = INVALID_HANDLE_VALUE;
			}

			return *this;
		}

		ScopedFileHandle(const ScopedFileHandle& other) = delete;
		ScopedFileHandle& operator=(const ScopedFileHandle& other) = delete;

		bool IsValid() const
		{
			return fileHandle != INVALID_HANDLE_VALUE;
		}

		HANDLE Get() const
		{
			return fileHandle;
		}

	private:
		void Close()
		{
			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				::CloseHandle(fileHandle);
				fileHandle = INVALID_HANDLE_VALUE;
			}
		}

		HANDLE fileHandle = INVALID_HANDLE_VALUE;
	};

	class ScopedFindHandle final
	{
	public:
		explicit ScopedFindHandle(HANDLE findHandle)
			: findHandle(findHandle)
		{
		}

		~ScopedFindHandle()
		{
			if (IsValid())
			{
				::FindClose(findHandle);
			}
		}

		ScopedFindHandle(const ScopedFindHandle& other) = delete;
		ScopedFindHandle& operator=(const ScopedFindHandle& other) = delete;

		bool IsValid() const
		{
			return findHandle != INVALID_HANDLE_VALUE;
		}

		HANDLE Get() const
		{
			return findHandle;
		}

	private:
		HANDLE findHandle = INVALID_HANDLE_VALUE;
	};

	bool ConvertPathForWindows(const CString& path, std::wstring& outPath)
	{
		outPath.clear();

		if (path.empty())
		{
			return false;
		}

		const CString windowsPath = Separators::Convert(path, Separators::Engine, Separators::Windows);

		return WindowsString::ToWideString(windowsPath, outPath);
	}

	ScopedFileHandle OpenFile(
		const CString& path,
		DWORD desiredAccess,
		DWORD shareMode,
		DWORD creationDisposition,
		DWORD flagsAndAttributes)
	{
		std::wstring fileName;

		if (!ConvertPathForWindows(path, fileName))
		{
			return ScopedFileHandle(INVALID_HANDLE_VALUE);
		}

		return ScopedFileHandle(::CreateFileW(
			fileName.c_str(),
			desiredAccess,
			shareMode,
			nullptr,
			creationDisposition,
			flagsAndAttributes,
			nullptr));
	}

	bool GetAttributes(const CString& path, DWORD& outAttributes)
	{
		outAttributes = INVALID_FILE_ATTRIBUTES;

		std::wstring fileName;

		if (!ConvertPathForWindows(path, fileName))
		{
			return false;
		}

		outAttributes = ::GetFileAttributesW(fileName.c_str());

		return outAttributes != INVALID_FILE_ATTRIBUTES;
	}

	bool ReadAllBytesFromHandle(HANDLE fileHandle, std::vector<uint8>& outBytes)
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

			if (::ReadFile(fileHandle, outBytes.data() + offset, readSize, &bytesRead, nullptr) == FALSE)
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

	bool ReadBytesFromHandle(
		HANDLE fileHandle,
		uint64 maxSize,
		std::vector<uint8>& outBytes)
	{
		outBytes.clear();

		if (maxSize == 0)
		{
			return true;
		}

		const DWORD readSize = static_cast<DWORD>(
			std::min<uint64>(
				maxSize,
				static_cast<uint64>(std::numeric_limits<DWORD>::max())));

		outBytes.resize(readSize);

		DWORD bytesRead = 0;

		if (::ReadFile(fileHandle, outBytes.data(), readSize, &bytesRead, nullptr) == FALSE)
		{
			outBytes.clear();
			return false;
		}

		outBytes.resize(bytesRead);

		return true;
	}

	bool WriteAllBytesToHandle(HANDLE fileHandle, const std::vector<uint8>& bytes)
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

			if (::WriteFile(fileHandle, bytes.data() + offset, writeSize, &bytesWritten, nullptr) == FALSE)
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

	bool IsDotDirectoryEntry(const wchar_t* fileName)
	{
		return std::wcscmp(fileName, L".") == 0 ||
			std::wcscmp(fileName, L"..") == 0;
	}

	std::wstring MakeDirectorySearchPattern(const std::wstring& directoryName)
	{
		std::wstring searchPattern = directoryName;

		if (!searchPattern.empty())
		{
			const wchar_t lastCharacter = searchPattern.back();

			if (lastCharacter != L'\\' && lastCharacter != L'/')
			{
				searchPattern.push_back(L'\\');
			}
		}

		searchPattern.push_back(L'*');

		return searchPattern;
	}

	std::wstring MakeChildPath(const std::wstring& directoryName, const wchar_t* childName)
	{
		std::wstring childPath = directoryName;

		if (!childPath.empty())
		{
			const wchar_t lastCharacter = childPath.back();

			if (lastCharacter != L'\\' && lastCharacter != L'/')
			{
				childPath.push_back(L'\\');
			}
		}

		childPath += childName;

		return childPath;
	}

	bool DeleteDirectoryTree(const std::wstring& directoryName)
	{
		const std::wstring searchPattern = MakeDirectorySearchPattern(directoryName);

		WIN32_FIND_DATAW findData = {};

		ScopedFindHandle findHandle(::FindFirstFileW(
			searchPattern.c_str(),
			&findData));

		if (!findHandle.IsValid())
		{
			return false;
		}

		do
		{
			if (IsDotDirectoryEntry(findData.cFileName))
			{
				continue;
			}

			const std::wstring childPath = MakeChildPath(directoryName, findData.cFileName);
			const bool isDirectory =
				(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			const bool isReparsePoint =
				(findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

			if (isDirectory)
			{
				if (isReparsePoint)
				{
					if (::RemoveDirectoryW(childPath.c_str()) == FALSE)
					{
						return false;
					}
				}
				else if (!DeleteDirectoryTree(childPath))
				{
					return false;
				}

				continue;
			}

			if (::DeleteFileW(childPath.c_str()) == FALSE)
			{
				return false;
			}
		} while (::FindNextFileW(findHandle.Get(), &findData) != FALSE);

		if (::GetLastError() != ERROR_NO_MORE_FILES)
		{
			return false;
		}

		return ::RemoveDirectoryW(directoryName.c_str()) != FALSE;
	}
}

bool PlatformFile::ReadAllBytes(const CString& path, std::vector<uint8>& outBytes)
{
	outBytes.clear();

	ScopedFileHandle fileHandle = OpenFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL);

	if (!fileHandle.IsValid())
	{
		return false;
	}

	return ReadAllBytesFromHandle(fileHandle.Get(), outBytes);
}

bool PlatformFile::ReadHead(const CString& path, uint64 maxSize, std::vector<uint8>& outBytes)
{
	outBytes.clear();

	ScopedFileHandle fileHandle = OpenFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL);

	if (!fileHandle.IsValid())
	{
		return false;
	}

	return ReadBytesFromHandle(fileHandle.Get(), maxSize, outBytes);
}

bool PlatformFile::WriteAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	ScopedFileHandle fileHandle = OpenFile(
		path,
		GENERIC_WRITE,
		0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL);

	if (!fileHandle.IsValid())
	{
		return false;
	}

	return WriteAllBytesToHandle(fileHandle.Get(), bytes);
}

bool PlatformFile::AppendAllBytes(const CString& path, const std::vector<uint8>& bytes)
{
	ScopedFileHandle fileHandle = OpenFile(
		path,
		FILE_APPEND_DATA,
		FILE_SHARE_READ,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL);

	if (!fileHandle.IsValid())
	{
		return false;
	}

	return WriteAllBytesToHandle(fileHandle.Get(), bytes);
}

bool PlatformFile::Create(const CString& path)
{
	ScopedFileHandle fileHandle = OpenFile(
		path,
		GENERIC_WRITE,
		0,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL);

	return fileHandle.IsValid();
}

bool PlatformFile::GetSize(const CString& path, uint64& outSize)
{
	outSize = 0;

	std::wstring fileName;

	if (!ConvertPathForWindows(path, fileName))
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
	DWORD attributes = INVALID_FILE_ATTRIBUTES;

	if (!GetAttributes(path, attributes))
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool PlatformFile::Remove(const CString& path)
{
	std::wstring fileName;

	if (!ConvertPathForWindows(path, fileName))
	{
		return false;
	}

	return ::DeleteFileW(fileName.c_str()) != FALSE;
}

bool PlatformFile::DirectoryExists(const CString& path)
{
	DWORD attributes = INVALID_FILE_ATTRIBUTES;

	if (!GetAttributes(path, attributes))
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::MakeDirectory(const CString& path)
{
	std::wstring directoryName;

	if (!ConvertPathForWindows(path, directoryName))
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

	DWORD attributes = INVALID_FILE_ATTRIBUTES;

	if (!GetAttributes(path, attributes))
	{
		return false;
	}

	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool PlatformFile::DeleteDirectory(const CString& path)
{
	std::wstring directoryName;

	if (!ConvertPathForWindows(path, directoryName))
	{
		return false;
	}

	return ::RemoveDirectoryW(directoryName.c_str()) != FALSE;
}

bool PlatformFile::DeleteDirectoryRecursively(const CString& path)
{
	std::wstring directoryName;

	if (!ConvertPathForWindows(path, directoryName))
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

	if (!ConvertPathForWindows(sourcePath, sourceFileName) ||
		!ConvertPathForWindows(targetPath, targetFileName))
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

	if (!ConvertPathForWindows(sourcePath, sourceFileName) ||
		!ConvertPathForWindows(targetPath, targetFileName))
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
