#include "pch.h"
#include "Runtime/Public/File.h"
#include "Runtime/Public/Path.h"
#include "Utility/Public/TstringUtility.h"	
#include "Common/Public/Logger.h"

FILE* File::OpenFile(const TSTRING& path, const TSTRING& fileName, const TCHAR* mode)
{
	TSTRING fullPath = Path::Combine(path, fileName);
	FILE* file = nullptr;

#if defined(PLATFORM_WINDOWS)
	if (_wfopen_s(&file, fullPath.c_str(), mode) != 0 || file == nullptr)
	{
		return nullptr;
	}
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

void File::Create(const TSTRING& path, const TSTRING& fileName, const TCHAR* mode)
{
	TSTRING fullPath = Path::Combine(path, fileName);

	if (Exists(path, fileName))
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("Create File Failed : already Exists\n") + fullPath);
		return;
	}

	FILE* file = OpenFile(path, fileName, mode);

	if (!file)
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("Create File Failed : ") + fullPath);
		return;
	}

	fclose(file);
}

void File::Delete(const TSTRING& path, const TSTRING& fileName)
{
	TSTRING fullPath = Path::Combine(path, fileName);

#if defined(PLATFORM_WINDOWS)  
	if (_wremove(fullPath.c_str()) == 0)
#elif defined(PLATFORM_LINUX)  
	if (remove(fullPath.c_str()) == 0)
#endif  
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("File Delete Failed : ") + fullPath);
	}
	else
	{
		LOG_INTERNAL_T(LogLevel::Info, TstringUtility::CharToTString("File Deleted : ") + fullPath);
	}
}

bool File::ReadAll(const TSTRING& path, const TSTRING& fileName, TSTRING& outdata)
{
	FILE* file = OpenFile(path, fileName, TTEXT("rt"));
	TSTRING fullPath = Path::Combine(path, fileName);

	if (!file)
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("Read File Failed : Cannot Open\n") + fullPath);
		return false;
	}

	outdata.clear();

	TCHAR buffer[256];
	while (fgetws(buffer, sizeof(buffer) / sizeof(TCHAR), file))
	{
		outdata += buffer;
	}

	fclose(file);
	return true;
}

bool File::WriteAll(const TSTRING& path, const TSTRING& fileName, const TSTRING& data)
{
	FILE* file = OpenFile(path, fileName, TTEXT("wt"));

	TSTRING fullPath = Path::Combine(path, fileName);

	if (!file)
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("Write File Failed : Cannot Open\n") + fullPath);
		return false;
	}

	size_t written = fwrite(data.c_str(), sizeof(TCHAR), data.length(), file);

	fclose(file);

	if (written != data.length())
	{
		LOG_INTERNAL_T(LogLevel::Error, TstringUtility::CharToTString("Write File Failed : Incomplete Write\n") + fullPath);
		return false;
	}

	return true;
}

bool File::AppendData(const TSTRING& path, const TSTRING& fileName, const TSTRING& data)
{
	FILE* file = OpenFile(path, fileName, TTEXT("at")); // append text ¸ðµå

	TSTRING fullPath = Path::Combine(path, fileName);

	if (!file)
	{
		TSTRING errorMsg = TstringUtility::CharToTString("Append File Failed : Cannot Open\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	size_t written = fwrite(data.c_str(), sizeof(TCHAR), data.length(), file);
	fclose(file);

	if (written != data.length())
	{
		TSTRING errorMsg = TstringUtility::CharToTString("Append File Failed : Incomplete Write\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	return true;
}

bool File::ReadAllBytes(const TSTRING& path, const TSTRING& fileName, std::vector<char>& outBytes)
{
	TSTRING fullPath = Path::Combine(path, fileName);
	FILE* file = OpenFile(path, fileName, TTEXT("rb"));
	if (!file)
	{
		TSTRING errorMsg = TstringUtility::CharToTString("ReadAllBytes Failed : Cannot Open\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	rewind(file);

	if (fileSize <= 0)
	{
		fclose(file);
		TSTRING errorMsg = TstringUtility::CharToTString("ReadAllBytes Failed : File size 0\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	outBytes.resize(fileSize);
	size_t read = fread(outBytes.data(), sizeof(char), fileSize, file);
	fclose(file);

	if (read != fileSize)
	{
		TSTRING errorMsg = TstringUtility::CharToTString("ReadAllBytes Failed : Incomplete read\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	return true;
}

bool File::WriteAllBytes(const TSTRING& path, const TSTRING& fileName, const std::vector<char>& bytes)
{
	TSTRING fullPath = Path::Combine(path, fileName);
	FILE* file = OpenFile(path, fileName, TTEXT("wb"));
	if (!file)
	{
		TSTRING errorMsg = TstringUtility::CharToTString("WriteAllBytes Failed : Cannot Open\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}

	size_t written = fwrite(bytes.data(), sizeof(char), bytes.size(), file);
	fclose(file);

	if (written != bytes.size())
	{
		TSTRING errorMsg = TstringUtility::CharToTString("WriteAllBytes Failed : Incomplete Write\n") + fullPath;
		LOG_INTERNAL_T(LogLevel::Error, errorMsg);
		return false;
	}
	return true;
}