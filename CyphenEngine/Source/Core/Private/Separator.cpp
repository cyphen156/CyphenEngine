#include "pch.h"

#include <vector>

#include "Core/Public/Separator.h"

namespace
{
	constexpr CChar ENGINE_SEPARATOR = CTEXT("/")[0];
	constexpr CChar WINDOWS_SEPARATOR = CTEXT("\\")[0];
	constexpr CChar UNIX_SEPARATOR = CTEXT("/")[0];

	std::vector<CChar> separators =
	{
		ENGINE_SEPARATOR,
		WINDOWS_SEPARATOR,
		UNIX_SEPARATOR
	};
}

void Separators::Reset()
{
	separators.clear();
	separators.push_back(ENGINE_SEPARATOR);
	separators.push_back(WINDOWS_SEPARATOR);
	separators.push_back(UNIX_SEPARATOR);
}

void Separators::Refresh(const std::vector<CChar>& userSeparators)
{
	Reset();

	for (CChar separator : userSeparators)
	{
		separators.push_back(separator);
	}
}

bool Separators::IsValid(Separator separator)
{
	return static_cast<std::size_t>(separator) < separators.size();
}

CChar Separators::Resolve(Separator separator)
{
	if (!IsValid(separator))
	{
		return static_cast<CChar>(0);
	}

	return separators[separator];
}

bool Separators::IsRegistered(CChar separator)
{
	for (CChar registered : separators)
	{
		if (separator == registered)
		{
			return true;
		}
	}

	return false;
}

CString Separators::Convert(const CString& text, Separator sourceSeparator, Separator targetSeparator)
{
	if (!IsValid(sourceSeparator) || !IsValid(targetSeparator))
	{
		return text;
	}

	return Convert(text, Resolve(sourceSeparator), Resolve(targetSeparator));
}

CString Separators::Convert(const CString& text, CChar sourceSeparator, CChar targetSeparator)
{
	CString result = text;

	if (sourceSeparator == targetSeparator)
	{
		return result;
	}

	for (CChar& character : result)
	{
		if (character == sourceSeparator)
		{
			character = targetSeparator;
		}
	}

	return result;
}
