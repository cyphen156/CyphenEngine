#include "pch.h"
#include "Core/Public/Path.h"

namespace
{
	constexpr CChar ENGINE_PATH_SEPARATOR = CTEXT("/")[0];
	constexpr CChar COMPATIBLE_PATH_SEPARATOR = CTEXT("\\")[0];
	constexpr CChar EXTENSION_SEPARATOR = CTEXT(".")[0];

	bool IsAcceptedPathSeparator(CChar ch)
	{
		return ch == ENGINE_PATH_SEPARATOR ||
			ch == COMPATIBLE_PATH_SEPARATOR;
	}

	CString::size_type FindLastEnginePathSeparator(const CString& path)
	{
		CString::size_type index = path.length();

		while (index > 0)
		{
			--index;

			if (path[index] == ENGINE_PATH_SEPARATOR)
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
	CString result;
	result.reserve(path.length());

	bool wasPreviousPathSeparator = false;

	for (CString::size_type i = 0; i < path.length(); ++i)
	{
		const CChar ch = path[i];

		if (IsAcceptedPathSeparator(ch))
		{
			if (!wasPreviousPathSeparator)
			{
				result.push_back(ENGINE_PATH_SEPARATOR);
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
	if (left.empty())
	{
		return right;
	}

	if (right.empty())
	{
		return left;
	}

	CString::size_type leftEnd = left.length();

	while (leftEnd > 0 && left[leftEnd - 1] == ENGINE_PATH_SEPARATOR)
	{
		--leftEnd;
	}

	CString::size_type rightStart = 0;

	while (rightStart < right.length() &&
		right[rightStart] == ENGINE_PATH_SEPARATOR)
	{
		++rightStart;
	}

	if (leftEnd == 0)
	{
		return CString(1, ENGINE_PATH_SEPARATOR) +
			right.substr(rightStart);
	}

	if (rightStart >= right.length())
	{
		return left.substr(0, leftEnd);
	}

	return left.substr(0, leftEnd) +
		ENGINE_PATH_SEPARATOR +
		right.substr(rightStart);
}

CString Path::GetDirectoryName(const CString& path)
{
	const CString::size_type separatorIndex = FindLastEnginePathSeparator(path);

	if (separatorIndex == CString::npos)
	{
		return CString();
	}

	if (separatorIndex == 0)
	{
		return CString(1, ENGINE_PATH_SEPARATOR);
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
