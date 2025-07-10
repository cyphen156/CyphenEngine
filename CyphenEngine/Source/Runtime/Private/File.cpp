#include "pch.h"
#include "Runtime/Public/File.h"
#include "Runtime/Public/Path.h"

FILE* OpenFile(const TSTRING& path, const TSTRING& fileName)
{
	TSTRING fullPath = Path::Combine(path, fileName);

#if defined(PLATFORM_WINDOWS)
	FILE* file = _wfopen(fullPath.c_str(), L"rb");
#elif defined(PLATFORM_LINUX)
	FILE* file = fopen(fullPath.c_str(), "rb");
#endif
	return file;
}

bool File::Exists(const TSTRING& path, const TSTRING& fileName)
{
	FILE* file = OpenFile(path, fileName);

	if (file)
	{
		fclose(file);
		return true;
	}
	return false;
}

void File::Create(const TSTRING& path, const TSTRING& fileName)
{

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
