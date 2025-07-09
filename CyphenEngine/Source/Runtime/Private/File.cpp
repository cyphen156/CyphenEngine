#include "pch.h"
#include "Runtime/Public/File.h"

bool File::Exists(const TSTRING& path, const TSTRING& fileName)
{
	//FILE* file = fopen("fileName", "r");
	/*if (file == nullptr)
	{
		return false;
	}*/
	return true;
}

void File::Create(const TSTRING& path, const TSTRING& fileName)
{

}

bool File::Delete(const TSTRING& path, const TSTRING& fileName)
{
	return false;
}

bool File::ReadAll(const TSTRING& path, const TSTRING& fileName, TSTRING& outText)
{
	return false;
}

bool File::WriteAll(const TSTRING& path, const TSTRING& fileName, const TSTRING& text)
{
	return false;
}

bool File::AddText(const TSTRING& path, const TSTRING& fileName, const TSTRING& text)
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
