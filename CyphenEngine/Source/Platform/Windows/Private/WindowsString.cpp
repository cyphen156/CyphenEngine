#include "pch.h"

#include <limits>
#include <string>

#include "Platform/Windows/Private/WindowsString.h"

bool WindowsString::ToWideString(
	const CString& text,
	std::wstring& outText)
{
	outText.clear();

#if CCHAR_IS_WCHAR

	outText = text;
	return true;

#elif CCHAR_IS_UTF16

	outText.reserve(text.length());

	for (CString::size_type i = 0; i < text.length(); ++i)
	{
		outText.push_back(static_cast<wchar_t>(text[i]));
	}

	return true;

#elif CCHAR_IS_UTF8

	if (text.empty())
	{
		return true;
	}

	if (text.length() >
		static_cast<CString::size_type>(std::numeric_limits<int>::max()))
	{
		return false;
	}

	const int sourceLength = static_cast<int>(text.length());

	const int requiredLength = ::MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		text.c_str(),
		sourceLength,
		nullptr,
		0);

	if (requiredLength <= 0)
	{
		return false;
	}

	outText.resize(static_cast<std::size_t>(requiredLength));

	const int convertedLength = ::MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		text.c_str(),
		sourceLength,
		&outText[0],
		requiredLength);

	if (convertedLength != requiredLength)
	{
		outText.clear();
		return false;
	}

	return true;

#else

	return false;

#endif
}
