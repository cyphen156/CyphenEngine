#include "pch.h"

#include "Platform/Windows/Private/WindowsPath.h"

#include "HAL/Private/PlatformPath.h"
#include "Platform/Windows/Private/WindowsString.h"

bool WindowsPath::ToNativePath(const CString& path, std::wstring& outPath)
{
	outPath.clear();

	CString platformPath;

	if (!PlatformPath::ToPlatformPath(path, platformPath))
	{
		return false;
	}

	return WindowsString::ToWideString(platformPath, outPath);
}

std::wstring WindowsPath::JoinNativePath(const std::wstring& parent, const wchar_t* child)
{
	std::wstring joined = parent;

	if (!joined.empty())
	{
		const wchar_t lastCharacter = joined.back();

		if (lastCharacter != L'\\' && lastCharacter != L'/')
		{
			joined.push_back(L'\\');
		}
	}

	joined += child;

	return joined;
}
