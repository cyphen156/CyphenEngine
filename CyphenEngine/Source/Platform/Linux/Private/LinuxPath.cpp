#include "pch.h"

#include "Platform/Linux/Private/LinuxPath.h"

#include "HAL/Private/PlatformPath.h"
#include "Platform/Linux/Private/LinuxString.h"

bool LinuxPath::ToNativePath(const CString& path, std::string& outPath)
{
	outPath.clear();

	CString platformPath;

	if (!PlatformPath::ToPlatformPath(path, platformPath))
	{
		return false;
	}

	return LinuxString::ToUtf8String(platformPath, outPath);
}

std::string LinuxPath::JoinNativePath(const std::string& parent, const char* child)
{
	std::string joined = parent;

	if (!joined.empty())
	{
		if (joined.back() != '/')
		{
			joined.push_back('/');
		}
	}

	joined += child;

	return joined;
}
