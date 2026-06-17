#include "pch.h"

#include "Core/Public/FileSystem.h"
#include "Core/Public/Path.h"
#include "Core/Public/Separator.h"
#include "HAL/Private/PlatformFile.h"

bool FileSystem::FileExists(const CString& path)
{
	return PlatformFile::Exists(path);
}

bool FileSystem::DirectoryExists(const CString& path)
{
	return PlatformFile::DirectoryExists(path);
}

bool FileSystem::Create(const CString& path)
{
	return PlatformFile::Create(path);
}

bool FileSystem::GetSize(const CString& path, uint64& outSize)
{
	return PlatformFile::GetSize(path, outSize);
}

bool FileSystem::RemoveFile(const CString& path)
{
	return PlatformFile::Remove(path);
}

bool FileSystem::MakeDirectory(const CString& path)
{
	return PlatformFile::MakeDirectory(path);
}

bool FileSystem::MakeDirectoryTree(const CString& path)
{
	const CString normalizedPath = Path::Normalize(path);

	if (normalizedPath.empty())
	{
		return false;
	}

	const CChar engineSeparator = Separators::Resolve(Separators::Engine);
	CString currentPath;

	for (CChar character : normalizedPath)
	{
		currentPath.push_back(character);

		if (character != engineSeparator)
		{
			continue;
		}

		if (currentPath.length() <= 1)
		{
			continue;
		}

		const CString directoryPath = currentPath.substr(0, currentPath.length() - 1);

		if (!PlatformFile::MakeDirectory(directoryPath))
		{
			return false;
		}
	}

	return PlatformFile::MakeDirectory(currentPath);
}

bool FileSystem::DeleteDirectory(const CString& path)
{
	return PlatformFile::DeleteDirectory(path);
}

bool FileSystem::DeleteDirectoryRecursively(const CString& path)
{
	return PlatformFile::DeleteDirectoryRecursively(path);
}

bool FileSystem::Copy(const CString& sourcePath, const CString& targetPath)
{
	return Copy(sourcePath, targetPath, false);
}

bool FileSystem::Copy(const CString& sourcePath, const CString& targetPath,
	bool canReplace)
{
	return PlatformFile::Copy(sourcePath, targetPath, canReplace);
}

bool FileSystem::Move(const CString& sourcePath, const CString& targetPath)
{
	return Move(sourcePath, targetPath, false);
}

bool FileSystem::Move(const CString& sourcePath, const CString& targetPath,
	bool canReplace)
{
	return PlatformFile::Move(sourcePath, targetPath, canReplace);
}
