#include "pch.h"
#include "Runtime/Public/File.h"
#include "Runtime/Public/Path.h"

FILE* File::OpenFile(const TSTRING& path, const TSTRING& fileName, const TCHAR* mode)
{
	TSTRING fullPath = Path::Combine(path, fileName);

#if defined(PLATFORM_WINDOWS)
	FILE* file = _wfopen(fullPath.c_str(), mode);
#elif defined(PLATFORM_LINUX)
	FILE* file = fopen(fullPath.c_str(), mode);
#endif
	return file;
}

bool File::Exists(const TSTRING& path, const TSTRING& fileName)
{
	FILE* file = OpenFile(path, fileName, TTEXT("rb"));

	if (file)
	{
		fclose(file);
		return true;
	}
	return false;
}

void File::Create(const TSTRING& path, const TSTRING& fileName)
{
	FILE* file = OpenFile(path, fileName, TTEXT("wb"));

	if (!file)
	{
		TSTRING fullPath = Path::Combine(path, fileName);
		LOG_INTERNAL(LogLevel::Error, "Create File Failed : " + fullPath);
	}
	
	if (Exists(path, fileName))
	{
		TSTRING fullPath = Path::Combine(path, fileName);
		LOG_INTERNAL(LogLevel::Error, "Create File Failed : " + fullPath);
	}

	fclose(file);
}

bool File::Delete(const TSTRING& path, const TSTRING& fileName)
{
	return false;
}

bool File::ReadAll(const TSTRING& path, const TSTRING& fileName, TSTRING& outdata)
{
	return false;
}

bool File::WriteAll(const TSTRING& path, const TSTRING& fileName, const TSTRING& data)
{
	return false;
}

bool File::AppendData(const TSTRING& path, const TSTRING& fileName, const TSTRING& data)
{
	return false;
}

bool File::ReadAllBytes(const TSTRING& path, const TSTRING& fileName, std::vector<char>& outBytes)
{
	return false;
}

bool File::WriteAllBytes(const TSTRING& path, const TSTRING& fileName, const std::vector<char>& bytes)
{
	return false;
}
