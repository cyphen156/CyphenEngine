#include "pch.h"

#include "Core/Public/FileSystem.h"
#include "HAL/Private/PlatformFile.h"

bool FileSystem::FileExists(const CString& path)
{
	return PlatformFile::FileExists(path);
}

bool FileSystem::DirectoryExists(const CString& path)
{
	return PlatformFile::DirectoryExists(path);
}

bool FileSystem::RemoveFile(const CString& path)
{
	return PlatformFile::RemoveFile(path);
}

bool FileSystem::MakeDirectory(const CString& path)
{
	return PlatformFile::MakeDirectory(path);
}

bool FileSystem::DeleteDirectory(const CString& path)
{
	return PlatformFile::DeleteDirectory(path);
}
