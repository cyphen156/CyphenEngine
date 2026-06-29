#include "pch.h"

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "HAL/Private/PlatformFile.h"
#include "Platform/Linux/Private/LinuxPath.h"

namespace
{
	constexpr std::uintptr_t InvalidFileHandleValue =
		std::numeric_limits<std::uintptr_t>::max();

	int ToNativeFd(FileHandle handle)
	{
		return static_cast<int>(handle.value);
	}

	bool DeleteDirectoryTree(const std::string& directoryName)
	{
		DIR* directory = ::opendir(directoryName.c_str());

		if (directory == nullptr)
		{
			return false;
		}

		bool result = true;

		while (result)
		{
			errno = 0;
			const dirent* entry = ::readdir(directory);

			if (entry == nullptr)
			{
				if (errno != 0)
				{
					result = false;
				}

				break;
			}

			if (std::strcmp(entry->d_name, ".") == 0 ||
				std::strcmp(entry->d_name, "..") == 0)
			{
				continue;
			}

			const std::string childPath = LinuxPath::JoinNativePath(directoryName, entry->d_name);

			struct stat entryStat = {};

			// lstat: 심볼릭 링크를 따라가지 않음 → 링크는 unlink, 실디렉터리만 재귀
			// (Windows reparse-point 안전 삭제와 동일 정책).
			if (::lstat(childPath.c_str(), &entryStat) != 0)
			{
				result = false;
				break;
			}

			if (S_ISDIR(entryStat.st_mode))
			{
				result = DeleteDirectoryTree(childPath);
			}
			else
			{
				result = ::unlink(childPath.c_str()) == 0;
			}
		}

		::closedir(directory);

		return result && ::rmdir(directoryName.c_str()) == 0;
	}
}

bool PlatformFile::OpenRead(const CString& path, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	const int fileDescriptor = ::open(fileName.c_str(), O_RDONLY);

	if (fileDescriptor < 0)
	{
		return false;
	}

	outHandle.value = static_cast<std::uintptr_t>(fileDescriptor);

	return true;
}

bool PlatformFile::OpenWrite(const CString& path, bool canReplace, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	// canReplace=true  -> CREATE_ALWAYS 등가 (O_TRUNC)
	// canReplace=false -> CREATE_NEW 등가    (O_EXCL, 존재 시 실패)
	const int flags = O_WRONLY | O_CREAT | (canReplace ? O_TRUNC : O_EXCL);

	const int fileDescriptor = ::open(fileName.c_str(), flags, 0644);

	if (fileDescriptor < 0)
	{
		return false;
	}

	outHandle.value = static_cast<std::uintptr_t>(fileDescriptor);

	return true;
}

bool PlatformFile::OpenAppend(const CString& path, FileHandle& outHandle)
{
	outHandle.value = InvalidFileHandleValue;

	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	const int fileDescriptor = ::open(fileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);

	if (fileDescriptor < 0)
	{
		return false;
	}

	outHandle.value = static_cast<std::uintptr_t>(fileDescriptor);

	return true;
}

void PlatformFile::Close(FileHandle& handle)
{
	if (handle.value == InvalidFileHandleValue)
	{
		return;
	}

	::close(ToNativeFd(handle));

	handle.value = InvalidFileHandleValue;
}

bool PlatformFile::Read(FileHandle handle, uint8* outBytes, uint64 bytesToRead, uint64& outBytesRead)
{
	outBytesRead = 0;

	if (handle.value == InvalidFileHandleValue ||
		(outBytes == nullptr && bytesToRead > 0))
	{
		return false;
	}

	const size_t readSize = static_cast<size_t>(
		std::min<uint64>(
			bytesToRead,
			static_cast<uint64>(std::numeric_limits<ssize_t>::max())));

	const ssize_t bytesRead = ::read(ToNativeFd(handle), outBytes, readSize);

	if (bytesRead < 0)
	{
		return false;
	}

	outBytesRead = static_cast<uint64>(bytesRead);

	return true;
}

bool PlatformFile::Write(FileHandle handle, const uint8* bytes, uint64 bytesToWrite, uint64& outBytesWritten)
{
	outBytesWritten = 0;

	if (handle.value == InvalidFileHandleValue ||
		(bytes == nullptr && bytesToWrite > 0))
	{
		return false;
	}

	const size_t writeSize = static_cast<size_t>(
		std::min<uint64>(
			bytesToWrite,
			static_cast<uint64>(std::numeric_limits<ssize_t>::max())));

	const ssize_t bytesWritten = ::write(ToNativeFd(handle), bytes, writeSize);

	if (bytesWritten < 0)
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

	if (position > static_cast<uint64>(std::numeric_limits<off_t>::max()))
	{
		return false;
	}

	return ::lseek(ToNativeFd(handle), static_cast<off_t>(position), SEEK_SET) >= 0;
}

bool PlatformFile::Tell(FileHandle handle, uint64& outPosition)
{
	outPosition = 0;

	if (handle.value == InvalidFileHandleValue)
	{
		return false;
	}

	const off_t position = ::lseek(ToNativeFd(handle), 0, SEEK_CUR);

	if (position < 0)
	{
		return false;
	}

	outPosition = static_cast<uint64>(position);

	return true;
}

bool PlatformFile::GetSize(FileHandle handle, uint64& outSize)
{
	outSize = 0;

	if (handle.value == InvalidFileHandleValue)
	{
		return false;
	}

	struct stat fileStat = {};

	if (::fstat(ToNativeFd(handle), &fileStat) != 0 ||
		fileStat.st_size < 0)
	{
		return false;
	}

	outSize = static_cast<uint64>(fileStat.st_size);

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

	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	struct stat fileStat = {};

	if (::stat(fileName.c_str(), &fileStat) != 0)
	{
		return false;
	}

	if (S_ISDIR(fileStat.st_mode) || fileStat.st_size < 0)
	{
		return false;
	}

	outSize = static_cast<uint64>(fileStat.st_size);

	return true;
}

bool PlatformFile::Exists(const CString& path)
{
	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	struct stat fileStat = {};

	if (::stat(fileName.c_str(), &fileStat) != 0)
	{
		return false;
	}

	return !S_ISDIR(fileStat.st_mode);
}

bool PlatformFile::Remove(const CString& path)
{
	std::string fileName;

	if (!LinuxPath::ToNativePath(path, fileName))
	{
		return false;
	}

	return ::unlink(fileName.c_str()) == 0;
}

bool PlatformFile::DirectoryExists(const CString& path)
{
	std::string directoryName;

	if (!LinuxPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	struct stat directoryStat = {};

	if (::stat(directoryName.c_str(), &directoryStat) != 0)
	{
		return false;
	}

	return S_ISDIR(directoryStat.st_mode);
}

bool PlatformFile::MakeDirectory(const CString& path)
{
	std::string directoryName;

	if (!LinuxPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	if (::mkdir(directoryName.c_str(), 0777) == 0)
	{
		return true;
	}

	if (errno != EEXIST)
	{
		return false;
	}

	struct stat directoryStat = {};

	if (::stat(directoryName.c_str(), &directoryStat) != 0)
	{
		return false;
	}

	return S_ISDIR(directoryStat.st_mode);
}

bool PlatformFile::DeleteDirectory(const CString& path)
{
	std::string directoryName;

	if (!LinuxPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	return ::rmdir(directoryName.c_str()) == 0;
}

bool PlatformFile::DeleteDirectoryRecursively(const CString& path)
{
	std::string directoryName;

	if (!LinuxPath::ToNativePath(path, directoryName))
	{
		return false;
	}

	return DeleteDirectoryTree(directoryName);
}

bool PlatformFile::Copy(const CString& sourcePath, const CString& targetPath, bool canReplace)
{
	std::string sourceFileName;
	std::string targetFileName;

	if (!LinuxPath::ToNativePath(sourcePath, sourceFileName) ||
		!LinuxPath::ToNativePath(targetPath, targetFileName))
	{
		return false;
	}

	const int sourceDescriptor = ::open(sourceFileName.c_str(), O_RDONLY);

	if (sourceDescriptor < 0)
	{
		return false;
	}

	struct stat sourceStat = {};

	if (::fstat(sourceDescriptor, &sourceStat) != 0)
	{
		::close(sourceDescriptor);
		return false;
	}

	const int targetFlags = O_WRONLY | O_CREAT | (canReplace ? O_TRUNC : O_EXCL);

	const int targetDescriptor = ::open(targetFileName.c_str(), targetFlags, sourceStat.st_mode & 0777);

	if (targetDescriptor < 0)
	{
		::close(sourceDescriptor);
		return false;
	}

	bool result = true;
	uint8 buffer[64 * 1024];

	while (true)
	{
		const ssize_t bytesRead = ::read(sourceDescriptor, buffer, sizeof(buffer));

		if (bytesRead < 0)
		{
			result = false;
			break;
		}

		if (bytesRead == 0)
		{
			break;
		}

		ssize_t offset = 0;

		while (offset < bytesRead)
		{
			const ssize_t bytesWritten = ::write(
				targetDescriptor,
				buffer + offset,
				static_cast<size_t>(bytesRead - offset));

			if (bytesWritten <= 0)
			{
				result = false;
				break;
			}

			offset += bytesWritten;
		}

		if (!result)
		{
			break;
		}
	}

	::close(sourceDescriptor);
	::close(targetDescriptor);

	return result;
}

bool PlatformFile::Move(const CString& sourcePath, const CString& targetPath, bool canReplace)
{
	std::string sourceFileName;
	std::string targetFileName;

	if (!LinuxPath::ToNativePath(sourcePath, sourceFileName) ||
		!LinuxPath::ToNativePath(targetPath, targetFileName))
	{
		return false;
	}

	if (!canReplace)
	{
		struct stat targetStat = {};

		if (::stat(targetFileName.c_str(), &targetStat) == 0)
		{
			return false;   // 대상 존재 + 교체 불가
		}
	}

	if (::rename(sourceFileName.c_str(), targetFileName.c_str()) == 0)
	{
		return true;
	}

	// 다른 파일시스템 간 이동: rename 불가(EXDEV) → copy 후 원본 삭제 (Windows MOVEFILE_COPY_ALLOWED 등가)
	if (errno != EXDEV)
	{
		return false;
	}

	if (!Copy(sourcePath, targetPath, canReplace))
	{
		return false;
	}

	return ::unlink(sourceFileName.c_str()) == 0;
}
