#include "pch.h"

#include "HAL/Private/PlatformPath.h"

#include "Core/Public/Separator.h"

bool PlatformPath::ToPlatformPath(const CString& enginePath, CString& outPlatformPath)
{
	outPlatformPath.clear();

	if (enginePath.empty())
	{
		return false;
	}

	outPlatformPath =
		Separators::Convert(enginePath, Separators::Engine, Separators::Unix);

	return true;
}
