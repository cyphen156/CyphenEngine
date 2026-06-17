#include "pch.h"

#include "Core/Public/Path.h"
#include "Core/Public/Separator.h"

namespace
{
	constexpr CChar EXTENSION_SEPARATOR = CTEXT(".")[0];

	CString::size_type FindLastEnginePathSeparator(const CString& path)
	{
		const CChar engineSeparator = Separators::Resolve(Separators::Engine);

		CString::size_type index = path.length();

		while (index > 0)
		{
			--index;

			if (path[index] == engineSeparator)
			{
				return index;
			}
		}

		return CString::npos;
	}

	CString::size_type FindExtensionSeparator(const CString& fileName)
	{
		CString::size_type index = fileName.length();

		while (index > 0)
		{
			--index;

			if (fileName[index] == EXTENSION_SEPARATOR)
			{
				if (index == 0)
				{
					return CString::npos;
				}

				if (index + 1 >= fileName.length())
				{
					return CString::npos;
				}

				return index;
			}
		}

		return CString::npos;
	}
}

CString Path::Normalize(const CString& path)
{
	const CChar engineSeparator = Separators::Resolve(Separators::Engine);

	CString result;
	result.reserve(path.length());

	bool wasPreviousPathSeparator = false;

	for (CString::size_type i = 0; i < path.length(); ++i)
	{
		const CChar ch = path[i];

		if (Separators::IsRegistered(ch))
		{
			if (!wasPreviousPathSeparator)
			{
				result.push_back(engineSeparator);
			}

			wasPreviousPathSeparator = true;
			continue;
		}

		result.push_back(ch);
		wasPreviousPathSeparator = false;
	}

	return result;
}

CString Path::Combine(const CString& left, const CString& right)
{
	const CChar engineSeparator = Separators::Resolve(Separators::Engine);

	if (left.empty())
	{
		return right;
	}

	if (right.empty())
	{
		return left;
	}

	CString::size_type leftEnd = left.length();

	while (leftEnd > 0 && left[leftEnd - 1] == engineSeparator)
	{
		--leftEnd;
	}

	CString::size_type rightStart = 0;

	while (rightStart < right.length() &&
		right[rightStart] == engineSeparator)
	{
		++rightStart;
	}

	if (leftEnd == 0)
	{
		return CString(1, engineSeparator) + right.substr(rightStart);
	}

	if (rightStart >= right.length())
	{
		return left.substr(0, leftEnd);
	}

	return left.substr(0, leftEnd) + engineSeparator + right.substr(rightStart);
}

CString Path::GetDirectoryName(const CString& path)
{
	const CChar engineSeparator = Separators::Resolve(Separators::Engine);
	const CString::size_type separatorIndex = FindLastEnginePathSeparator(path);

	if (separatorIndex == CString::npos)
	{
		return CString();
	}

	if (separatorIndex == 0)
	{
		return CString(1, engineSeparator);
	}

	return path.substr(0, separatorIndex);
}

CString Path::GetFileName(const CString& path)
{
	const CString::size_type separatorIndex = FindLastEnginePathSeparator(path);

	if (separatorIndex == CString::npos)
	{
		return path;
	}

	if (separatorIndex + 1 >= path.length())
	{
		return CString();
	}

	return path.substr(separatorIndex + 1);
}

CString Path::GetFileNameWithoutExtension(const CString& path)
{
	const CString fileName = GetFileName(path);
	const CString::size_type extensionIndex = FindExtensionSeparator(fileName);

	if (extensionIndex == CString::npos)
	{
		return fileName;
	}

	return fileName.substr(0, extensionIndex);
}

CString Path::GetExtension(const CString& path)
{
	const CString fileName = GetFileName(path);
	const CString::size_type extensionIndex = FindExtensionSeparator(fileName);

	if (extensionIndex == CString::npos)
	{
		return CString();
	}

	return fileName.substr(extensionIndex);
}
